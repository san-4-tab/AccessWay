// VolunteerManager.cpp  (Member 2)
#include "VolunteerManager.h"
#include <fstream>
#include <iomanip>
#include <iostream>
VolunteerManager::VolunteerManager() : head(nullptr), tail(nullptr), count(0) {}

VolunteerManager::~VolunteerManager() { clear(); }

void VolunteerManager::clear() {
    VolunteerNode* cur = head;
    while (cur != nullptr) {
        VolunteerNode* next = cur->next;
        delete cur;
        cur = next;
    }
    head = tail = nullptr;
    count = 0;
    index.clear();
}

bool VolunteerManager::registerVolunteer(const Volunteer& volunteer) {

    if (volunteer.volunteerId <= 0 ||
        volunteer.buildingId <= 0 ||
        volunteer.currentLocationId <= 0 ||
        volunteer.volunteerName.empty() ||
        volunteer.assistanceType.empty())
        return false;

    if (volunteer.volunteerName.find_first_of("|\r\n") != string::npos ||
        volunteer.assistanceType.find_first_of("|\r\n") != string::npos)
        return false;

    long long key = makeKey(
        volunteer.buildingId,
        volunteer.volunteerId
    );

    if (index.find(key) != index.end())
        return false;

    VolunteerNode* node = new VolunteerNode;
    node->data = volunteer;
    node->next = nullptr;

    if (tail == nullptr)
        head = tail = node;
    else {
        tail->next = node;
        tail = node;
    }

    index[key] = node;
    count++;

    return true;
}

bool VolunteerManager::removeVolunteer(int buildingId, int volunteerId) {
    long long key = makeKey(buildingId, volunteerId);
    if (index.find(key) == index.end()) return false;

    VolunteerNode* prev = nullptr;
    VolunteerNode* cur = head;
    while (cur != nullptr && cur != index[key]) { prev = cur; cur = cur->next; }
    if (cur == nullptr) return false;

    if (prev == nullptr) head = cur->next;
    else prev->next = cur->next;
    if (cur == tail) tail = prev;

    delete cur;
    index.erase(key);
    count--;
    return true;
}

Volunteer* VolunteerManager::findVolunteer(int buildingId, int volunteerId) {
    auto it = index.find(makeKey(buildingId, volunteerId));
    if (it == index.end()) return nullptr;
    return &(it->second->data);
}

bool VolunteerManager::setAvailability(int buildingId, int volunteerId, bool available) {
    Volunteer* v = findVolunteer(buildingId, volunteerId);
    if (!v) return false;
    v->available = available;
    return true;
}

bool VolunteerManager::updateLocation(int buildingId, int volunteerId, int locationId) {
    Volunteer* v = findVolunteer(buildingId, volunteerId);
    if (!v) return false;
    v->currentLocationId = locationId;
    return true;
}

vector<Volunteer> VolunteerManager::getAvailableVolunteers(int buildingId, const string& assistanceType) const {
    vector<Volunteer> result;
    for (VolunteerNode* cur = head; cur != nullptr; cur = cur->next) {
        const Volunteer& v = cur->data;
        if (v.buildingId != buildingId || !v.available) continue;
        if (!assistanceType.empty() && v.assistanceType != assistanceType) continue;
        result.push_back(v);
    }
    return result;
}

vector<Volunteer> VolunteerManager::getAllVolunteers(int buildingId) const {
    vector<Volunteer> result;
    for (VolunteerNode* cur = head; cur != nullptr; cur = cur->next)
        if (cur->data.buildingId == buildingId) result.push_back(cur->data);
    return result;
}

int VolunteerManager::getCount() const { return count; }

void VolunteerManager::displayVolunteers(int buildingId) const {
    cout << "\n--- Volunteers (Building " << buildingId << ") ---\n";
    cout << left << setw(5) << "ID" << setw(18) << "Name" << setw(16) << "Assistance"
         << setw(11) << "Available" << "Location\n";
    for (VolunteerNode* cur = head; cur != nullptr; cur = cur->next) {
        const Volunteer& v = cur->data;
        if (v.buildingId != buildingId) continue;
        cout << left << setw(5) << v.volunteerId << setw(18) << v.volunteerName
             << setw(16) << v.assistanceType << setw(11) << (v.available ? "Yes" : "No")
             << v.currentLocationId << "\n";
    }
}

bool VolunteerManager::saveToFile(const string& filename) const {
    ofstream out(filename.c_str());
    if (!out) return false;
    for (VolunteerNode* cur = head; cur != nullptr; cur = cur->next) {
        const Volunteer& v = cur->data;
        out << v.volunteerId << '|' << v.buildingId << '|' << v.volunteerName << '|'
            << v.assistanceType << '|' << (v.available ? 1 : 0) << '|'
            << v.currentLocationId << '\n';
    }
    return true;
}

bool VolunteerManager::loadFromFile(const string& filename) {

    ifstream in(filename.c_str());

    if (!in)
        return false;

    clear();

    string line;
    bool success = true;

    while (getline(in, line)) {

        if (line.empty())
            continue;

        vector<string> p = splitLine(line);

        if (p.size() != 6) {
            success = false;
            continue;
        }

        try {

            Volunteer v;

            size_t pos0 = 0;
            size_t pos1 = 0;
            size_t pos4 = 0;
            size_t pos5 = 0;

            v.volunteerId = stoi(p[0], &pos0);
            v.buildingId = stoi(p[1], &pos1);
            v.volunteerName = p[2];
            v.assistanceType = p[3];

            int availability = stoi(p[4], &pos4);

            v.currentLocationId = stoi(p[5], &pos5);

            if (pos0 != p[0].size() ||
                pos1 != p[1].size() ||
                pos4 != p[4].size() ||
                pos5 != p[5].size() ||
                (availability != 0 && availability != 1)) {

                success = false;
                continue;
            }

            v.available = (availability == 1);

            if (!registerVolunteer(v))
                success = false;

        }
        catch (...) {
            success = false;
        }
    }

    return success && !in.bad();
}
