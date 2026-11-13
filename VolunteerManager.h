// VolunteerManager.h  (Member 2)
// Volunteer registration + availability.
// DSA: singly linked list (storage) + hash table (O(1) lookup by building+id).
#ifndef VOLUNTEER_MANAGER_H
#define VOLUNTEER_MANAGER_H

#include <vector>
#include <string>
#include <unordered_map>
#include "../common/AccessWayData.h"

// A volunteerId / requestId is unique only INSIDE a building,
// so every hash key combines buildingId + id.
inline long long makeKey(int buildingId, int id) {
    return (static_cast<long long>(buildingId) << 32) | static_cast<unsigned int>(id);
}

// Splits "a|b|c" into {"a","b","c"} (used by file loading).
inline vector<string> splitLine(const string& line, char delim = '|') {
    vector<string> parts;
    string cur;
    for (size_t i = 0; i < line.size(); i++) {
        if (line[i] == delim) { parts.push_back(cur); cur.clear(); }
        else cur += line[i];
    }
    parts.push_back(cur);
    return parts;
}

// Node of the linked list
struct VolunteerNode {
    Volunteer data;
    VolunteerNode* next;
};

class VolunteerManager {
private:
    VolunteerNode* head;
    VolunteerNode* tail;
    int count;
    unordered_map<long long, VolunteerNode*> index;   // hash table

public:
    VolunteerManager();
    ~VolunteerManager();
    VolunteerManager(const VolunteerManager&) = delete;
    VolunteerManager& operator=(const VolunteerManager&) = delete;

    bool registerVolunteer(const Volunteer& volunteer);
    bool removeVolunteer(int buildingId, int volunteerId);

    bool setAvailability(int buildingId, int volunteerId, bool available);
    bool updateLocation(int buildingId, int volunteerId, int locationId);

    Volunteer* findVolunteer(int buildingId, int volunteerId);

    // assistanceType == "" returns every available volunteer of the building
    vector<Volunteer> getAvailableVolunteers(int buildingId, const string& assistanceType = "") const;
    vector<Volunteer> getAllVolunteers(int buildingId) const;

    int getCount() const;
    void displayVolunteers(int buildingId) const;
    void clear();

    // volunteers.txt  ->  id|buildingId|name|assistanceType|available|currentLocationId
    bool saveToFile(const string& filename) const;
    bool loadFromFile(const string& filename);
};

#endif
