// HelpRequestManager.cpp  (Member 2)
#include "HelpRequestManager.h"
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <iostream>
#include <cctype>

HelpRequestManager::HelpRequestManager(VolunteerManager& vm) : volunteers(vm), nextSequence(0) {}

// Urgency from the help type text
int HelpRequestManager::priorityOf(const string& helpType) {
    string t = helpType;
    transform(t.begin(), t.end(), t.begin(), ::tolower);
    if (t.find("emergency") != string::npos || t.find("medical") != string::npos) return 3;
    if (t.find("wheelchair") != string::npos || t.find("mobility") != string::npos) return 2;
    return 1;
}

bool HelpRequestManager::validTransition(RequestStatus from, RequestStatus to) {
    switch (from) {
        case PENDING:     return to == ACCEPTED || to == CANCELLED;
        case ACCEPTED:    return to == IN_PROGRESS || to == CANCELLED;
        case IN_PROGRESS: return to == COMPLETED || to == CANCELLED;
        default:          return false;                    // COMPLETED / CANCELLED are final
    }
}

string HelpRequestManager::statusToString(RequestStatus s) {
    switch (s) {
        case PENDING:     return "PENDING";
        case ACCEPTED:    return "ACCEPTED";
        case IN_PROGRESS: return "IN_PROGRESS";
        case COMPLETED:   return "COMPLETED";
        case CANCELLED:   return "CANCELLED";
    }
    return "UNKNOWN";
}


bool HelpRequestManager::createHelpRequest(const HelpRequest& request) {

    // Reject invalid IDs and empty help types.
    if (request.requestId <= 0 ||
        request.buildingId <= 0 ||
        request.visitorLocationId <= 0 ||
        request.helpType.empty())
        return false;

    // Prevent file-format corruption.
    if (request.helpType.find_first_of("|\r\n") != string::npos)
        return false;

    long long key = makeKey(request.buildingId, request.requestId);

    // Reject duplicate request IDs within the same building.
    if (requests.find(key) != requests.end())
        return false;

    HelpRequest r = request;

    // Every new request starts unassigned and pending.
    r.status = PENDING;
    r.assignedVolunteerId = -1;

    requests[key] = r;

    PendingEntry e;
    e.priority = priorityOf(r.helpType);
    e.sequence = nextSequence++;
    e.requestId = r.requestId;

    pendingQueues[r.buildingId].push(e);

    return true;
}


int HelpRequestManager::nextRequestId(int buildingId) const {
    int maxId = 0;
    for (auto& kv : requests)
        if (kv.second.buildingId == buildingId && kv.second.requestId > maxId) maxId = kv.second.requestId;
    return maxId + 1;
}

HelpRequest* HelpRequestManager::findRequest(int buildingId, int requestId) {
    auto it = requests.find(makeKey(buildingId, requestId));
    return it == requests.end() ? nullptr : &(it->second);
}

bool HelpRequestManager::assignVolunteer(int buildingId, int requestId, int volunteerId) {
    HelpRequest* r = findRequest(buildingId, requestId);
    Volunteer* v = volunteers.findVolunteer(buildingId, volunteerId);
    if (!r || !v) return false;
    if (r->status != PENDING || !v->available) return false;

    v->available = false;                  // volunteer is now busy
    r->assignedVolunteerId = volunteerId;
    r->status = ACCEPTED;
    return true;
}

bool HelpRequestManager::autoAssignRequest(int buildingId, int requestId, const EvacuationManager& evac) {
    HelpRequest* r = findRequest(buildingId, requestId);
    if (!r || r->status != PENDING) return false;

    // prefer volunteers trained for this help type, otherwise any free volunteer
    vector<Volunteer> candidates = volunteers.getAvailableVolunteers(buildingId, r->helpType);
    if (candidates.empty()) candidates = volunteers.getAvailableVolunteers(buildingId);

    int bestId = -1;
    float bestDist = 1e30f;
    for (size_t i = 0; i < candidates.size(); i++) {
        RouteResult route = evac.findRoute(buildingId, candidates[i].currentLocationId, r->visitorLocationId);
        if (route.found && route.totalDistance < bestDist) {
            bestDist = route.totalDistance;
            bestId = candidates[i].volunteerId;
        }
    }
    if (bestId == -1) return false;
    return assignVolunteer(buildingId, requestId, bestId);
}


bool HelpRequestManager::processNextRequest(
    int buildingId,
    const EvacuationManager& evac,
    int* handled
) {

    PendingQueue& pq = pendingQueues[buildingId];

    vector<PendingEntry> deferred;

    while (!pq.empty()) {

        PendingEntry e = pq.top();
        pq.pop();

        HelpRequest* r = findRequest(buildingId, e.requestId);

        // Ignore stale queue entries.
        if (!r || r->status != PENDING)
            continue;

        // Try assigning this request.
        if (autoAssignRequest(buildingId, e.requestId, evac)) {

            // Restore requests that could not be assigned.
            for (size_t i = 0; i < deferred.size(); i++)
                pq.push(deferred[i]);

            if (handled)
                *handled = e.requestId;

            return true;
        }

        // Keep this request pending and try the next one.
        deferred.push_back(e);
    }

    // Restore all requests that remain pending.
    for (size_t i = 0; i < deferred.size(); i++)
        pq.push(deferred[i]);

    return false;
}


bool HelpRequestManager::updateRequestStatus(int buildingId, int requestId, RequestStatus status) {
    HelpRequest* r = findRequest(buildingId, requestId);
    if (!r || !validTransition(r->status, status)) return false;
    if (status == ACCEPTED && r->assignedVolunteerId == -1) return false;   // use assignVolunteer()

    r->status = status;

    if ((status == COMPLETED || status == CANCELLED) && r->assignedVolunteerId != -1) {
        volunteers.setAvailability(buildingId, r->assignedVolunteerId, true);   // free the volunteer
        if (status == COMPLETED)
            volunteers.updateLocation(buildingId, r->assignedVolunteerId, r->visitorLocationId);
    }
    return true;
}

bool HelpRequestManager::cancelRequest(int buildingId, int requestId) {
    return updateRequestStatus(buildingId, requestId, CANCELLED);
}

vector<HelpRequest> HelpRequestManager::getRequestsByStatus(int buildingId, RequestStatus status) const {
    vector<HelpRequest> result;
    for (auto& kv : requests)
        if (kv.second.buildingId == buildingId && kv.second.status == status) result.push_back(kv.second);
    sort(result.begin(), result.end(),
         [](const HelpRequest& a, const HelpRequest& b) { return a.requestId < b.requestId; });
    return result;
}

vector<HelpRequest> HelpRequestManager::getAllRequests(int buildingId) const {
    vector<HelpRequest> result;
    for (auto& kv : requests)
        if (kv.second.buildingId == buildingId) result.push_back(kv.second);
    sort(result.begin(), result.end(),
         [](const HelpRequest& a, const HelpRequest& b) { return a.requestId < b.requestId; });
    return result;
}

int HelpRequestManager::countPending(int buildingId) const {
    return static_cast<int>(getRequestsByStatus(buildingId, PENDING).size());
}

void HelpRequestManager::displayRequests(int buildingId) const {
    cout << "\n--- Help Requests (Building " << buildingId << ") ---\n";
    cout << left << setw(5) << "ID" << setw(20) << "Help type" << setw(10) << "Visitor@"
         << setw(12) << "Volunteer" << "Status\n";
    vector<HelpRequest> all = getAllRequests(buildingId);
    for (size_t i = 0; i < all.size(); i++) {
        const HelpRequest& r = all[i];
        cout << left << setw(5) << r.requestId << setw(20) << r.helpType << setw(10) << r.visitorLocationId
             << setw(12) << (r.assignedVolunteerId == -1 ? string("-") : to_string(r.assignedVolunteerId))
             << statusToString(r.status) << "\n";
    }
}

bool HelpRequestManager::saveToFile(const string& filename) const {
    ofstream out(filename.c_str());
    if (!out) return false;
    for (auto& kv : requests) {
        const HelpRequest& r = kv.second;
        out << r.requestId << '|' << r.buildingId << '|' << r.visitorLocationId << '|'
            << r.assignedVolunteerId << '|' << r.helpType << '|' << static_cast<int>(r.status) << '\n';
    }
    return true;
}


bool HelpRequestManager::loadFromFile(const string& filename) {

    ifstream in(filename.c_str());

    if (!in)
        return false;

    requests.clear();
    pendingQueues.clear();
    nextSequence = 0;

    string line;
    bool success = true;

    while (getline(in, line)) {

        if (line.empty())
            continue;

        vector<string> p = splitLine(line);

        // Exactly six fields are required.
        if (p.size() != 6) {
            success = false;
            continue;
        }

        try {

            HelpRequest r;

            size_t pos0 = 0, pos1 = 0, pos2 = 0;
            size_t pos3 = 0, pos5 = 0;

            r.requestId = stoi(p[0], &pos0);
            r.buildingId = stoi(p[1], &pos1);
            r.visitorLocationId = stoi(p[2], &pos2);
            r.assignedVolunteerId = stoi(p[3], &pos3);
            r.helpType = p[4];

            int statusValue = stoi(p[5], &pos5);

            // Reject partially parsed numbers.
            if (pos0 != p[0].size() ||
                pos1 != p[1].size() ||
                pos2 != p[2].size() ||
                pos3 != p[3].size() ||
                pos5 != p[5].size()) {

                success = false;
                continue;
            }

            // Validate IDs and help type.
            if (r.requestId <= 0 ||
                r.buildingId <= 0 ||
                r.visitorLocationId <= 0 ||
                r.assignedVolunteerId < -1 ||
                r.helpType.empty() ||
                r.helpType.find_first_of("|\r\n") != string::npos) {

                success = false;
                continue;
            }

            // Validate request status.
            if (statusValue < PENDING || statusValue > CANCELLED) {
                success = false;
                continue;
            }

            r.status = static_cast<RequestStatus>(statusValue);

            // Active and completed requests must have an assigned volunteer.
            if ((r.status == ACCEPTED ||
                 r.status == IN_PROGRESS ||
                 r.status == COMPLETED) &&
                r.assignedVolunteerId == -1) {

                success = false;
                continue;
            }

            // Pending requests must not have an assigned volunteer.
            if (r.status == PENDING &&
                r.assignedVolunteerId != -1) {

                success = false;
                continue;
            }

            long long key = makeKey(r.buildingId, r.requestId);

            // Reject duplicate request IDs within a building.
            if (requests.count(key)) {
                success = false;
                continue;
            }

            requests[key] = r;

            // Rebuild pending priority queues.
            if (r.status == PENDING) {

                PendingEntry e;
                e.priority = priorityOf(r.helpType);
                e.sequence = nextSequence++;
                e.requestId = r.requestId;

                pendingQueues[r.buildingId].push(e);
            }

        }
        catch (...) {
            success = false;
        }
    }

    return success && !in.bad();
}

void HelpRequestManager::syncVolunteerAvailability() {

    for (auto& kv : requests) {

        const HelpRequest& r = kv.second;

        if ((r.status == ACCEPTED || r.status == IN_PROGRESS) &&
            r.assignedVolunteerId != -1) {

            volunteers.setAvailability(
                r.buildingId,
                r.assignedVolunteerId,
                false
            );
        }
    }
}

