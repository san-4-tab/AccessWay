// HelpRequestManager.h  (Member 2)
// Visitor help requests: create, prioritise, assign volunteer, track status.
// DSA: hash table (request lookup), priority queue (pending requests), queue-based BFS/Dijkstra
//      (through EvacuationManager) for choosing the nearest volunteer.
#ifndef HELP_REQUEST_MANAGER_H
#define HELP_REQUEST_MANAGER_H

#include <vector>
#include <queue>
#include <string>
#include <unordered_map>
#include "common/AccessWayData.h"
#include "common/EmergencyData.h"
#include "VolunteerManager.h"
#include "EvacuationManager.h"

struct PendingEntry {
    int priority;        // higher = more urgent
    long sequence;       // smaller = arrived earlier (FIFO among equal priority)
    int requestId;
};

struct PendingCompare {
    bool operator()(const PendingEntry& a, const PendingEntry& b) const {
        if (a.priority != b.priority) return a.priority < b.priority;
        return a.sequence > b.sequence;
    }
};

typedef priority_queue<PendingEntry, vector<PendingEntry>, PendingCompare> PendingQueue;

class HelpRequestManager {
private:
    VolunteerManager& volunteers;                        // shared with EmergencyManager
    unordered_map<long long, HelpRequest> requests;      // hash table (building+id)
    unordered_map<int, PendingQueue> pendingQueues;      // one priority queue per building
    long nextSequence;

    static int priorityOf(const string& helpType);
    static bool validTransition(RequestStatus from, RequestStatus to);

public:
    explicit HelpRequestManager(VolunteerManager& volunteerManager);

    bool createHelpRequest(const HelpRequest& request);
    int  nextRequestId(int buildingId) const;

    // manual assignment (volunteer accepts) -> status ACCEPTED
    bool assignVolunteer(int buildingId, int requestId, int volunteerId);
    // picks the nearest suitable available volunteer using route distance
    bool autoAssignRequest(int buildingId, int requestId, const EvacuationManager& evacuation);
    // takes the most urgent pending request and auto-assigns it
    bool processNextRequest(int buildingId, const EvacuationManager& evacuation, int* handledRequestId = nullptr);

    // PENDING -> ACCEPTED -> IN_PROGRESS -> COMPLETED (CANCELLED allowed before completion)
    bool updateRequestStatus(int buildingId, int requestId, RequestStatus status);
    bool cancelRequest(int buildingId, int requestId);

    HelpRequest* findRequest(int buildingId, int requestId);
    vector<HelpRequest> getRequestsByStatus(int buildingId, RequestStatus status) const;
    vector<HelpRequest> getAllRequests(int buildingId) const;
    int countPending(int buildingId) const;

    void displayRequests(int buildingId) const;
    static string statusToString(RequestStatus status);

    // requests.txt -> id|buildingId|visitorLocationId|assignedVolunteerId|helpType|status
    bool saveToFile(const string& filename) const;
    bool loadFromFile(const string& filename);
};

#endif
