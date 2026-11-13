
#ifndef ACCESSWAY_REVIEW_MANAGER_H
#define ACCESSWAY_REVIEW_MANAGER_H

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include "../common/AccessWayData.h"
using namespace std;

// Review data stored by the review linked list.

struct ReviewNode {
    Review data;
    ReviewNode* next;

    ReviewNode(Review r) {
        data = r;
        next = nullptr;
    }
};

class ReviewManager {
private:
    ReviewNode* head;

public:
    ReviewManager() {
        head = nullptr;
    }

    ~ReviewManager() {
        clear();
    }

    void clear() {
        while (head != nullptr) {
            ReviewNode* temp = head;
            head = head->next;
            delete temp;
        }
    }

    bool reviewExists(int id) const {
        ReviewNode* temp = head;

        while (temp != nullptr) {
            if (temp->data.reviewId == id)
                return true;

            temp = temp->next;
        }

        return false;
    }

    void addReview(Review r) {
        if (reviewExists(r.reviewId)) {
            cout << "Review ID already exists.\n";
            return;
        }

        ReviewNode* newNode = new ReviewNode(r);

        if (head == nullptr) {
            head = newNode;
            return;
        }

        ReviewNode* temp = head;

        while (temp->next != nullptr)
            temp = temp->next;

        temp->next = newNode;
    }

    void displayReviews() const {
        if (head == nullptr) {
            cout << "\nNo reviews found.\n";
            return;
        }

        ReviewNode* temp = head;

        cout << "\n========== REVIEWS ==========\n";

        while (temp != nullptr) {
            Review r = temp->data;

            cout << "\nReview ID: " << r.reviewId;
            cout << "\nBuilding ID: " << r.buildingId;
            cout << "\nLocation ID: " << r.locationId;
            cout << "\nCategory: " << r.category;
            cout << "\nSeverity: " << r.severity;
            cout << "\nDescription: " << r.description;
            cout << "\nVerified: "
                 << (r.verified ? "Yes" : "No");
            cout << "\n-----------------------------\n";

            temp = temp->next;
        }
    }

    bool verifyReview(int id) {
        ReviewNode* temp = head;

        while (temp != nullptr) {
            if (temp->data.reviewId == id) {
                temp->data.verified = true;
                return true;
            }

            temp = temp->next;
        }

        return false;
    }

    vector<Review> getReviews(int buildingId) const {
        vector<Review> result;

        ReviewNode* temp = head;

        while (temp != nullptr) {
            if (temp->data.buildingId == buildingId)
                result.push_back(temp->data);

            temp = temp->next;
        }

        return result;
    }

    void saveToFile() const {
        ofstream file("reviews.txt");

        if (!file) {
            cout << "Unable to save file.\n";
            return;
        }

        ReviewNode* temp = head;

        while (temp != nullptr) {
            Review r = temp->data;

            file << r.reviewId << "|"
                 << r.buildingId << "|"
                 << r.locationId << "|"
                 << r.severity << "|"
                 << r.category << "|"
                 << r.description << "|"
                 << r.verified << "\n";

            temp = temp->next;
        }

        file.close();
    }

    void loadFromFile() {
        ifstream file("reviews.txt");

        if (!file)
            return;

        clear();

        string line;

        while (getline(file, line)) {
            vector<string> fields;
            string current;

            for (char ch : line) {
                if (ch == '|') {
                    fields.push_back(current);
                    current.clear();
                } else {
                    current += ch;
                }
            }

            fields.push_back(current);

            if (fields.size() != 7)
                continue;

            try {
                Review r;

                r.reviewId = stoi(fields[0]);
                r.buildingId = stoi(fields[1]);
                r.locationId = stoi(fields[2]);
                r.severity = stoi(fields[3]);
                r.category = fields[4];
                r.description = fields[5];
                r.verified = (fields[6] == "1");

                addReview(r);
            }
            catch (...) {
                cout << "Skipping invalid review record.\n";
            }
        }

        file.close();
    }
};

#endif
