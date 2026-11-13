
#ifndef ACCESSWAY_REVIEW_OPTIMIZER_H
#define ACCESSWAY_REVIEW_OPTIMIZER_H

#include <iostream>
#include <vector>
#include <algorithm>
#include "ReviewManager.h"

using namespace std;

struct ProblemSummary {
    int locationId;
    int reviewCount;
    int verifiedCount;
    int severityTotal;
    int priorityScore;
};

class MaxHeap {
private:
    vector<ProblemSummary> heap;

    bool higherPriority(
        const ProblemSummary& a,
        const ProblemSummary& b
    ) const {
        if (a.priorityScore != b.priorityScore)
            return a.priorityScore > b.priorityScore;

        return a.locationId < b.locationId;
    }

    void heapifyUp(int i) {
        while (i > 0) {
            int parent = (i - 1) / 2;

            if (higherPriority(heap[i], heap[parent])) {
                swap(heap[i], heap[parent]);
                i = parent;
            } else {
                break;
            }
        }
    }

    void heapifyDown(int i) {
        int n = static_cast<int>(heap.size());

        while (true) {
            int left = 2 * i + 1;
            int right = 2 * i + 2;
            int largest = i;

            if (left < n &&
                higherPriority(heap[left], heap[largest]))
                largest = left;

            if (right < n &&
                higherPriority(heap[right], heap[largest]))
                largest = right;

            if (largest == i)
                break;

            swap(heap[i], heap[largest]);
            i = largest;
        }
    }

public:
    void push(const ProblemSummary& p) {
        heap.push_back(p);
        heapifyUp(static_cast<int>(heap.size()) - 1);
    }

    bool empty() const {
        return heap.empty();
    }

    ProblemSummary pop() {
        if (empty())
            return ProblemSummary{};

        ProblemSummary result = heap[0];

        heap[0] = heap.back();
        heap.pop_back();

        if (!heap.empty())
            heapifyDown(0);

        return result;
    }
};

class ReviewOptimizer {
public:
    void generateRanking(
        const vector<Review>& reviews,
        int buildingId
    ) {
        vector<ProblemSummary> summaries;

        for (const Review& r : reviews) {
            if (r.buildingId != buildingId)
                continue;

            int index = -1;

            for (int i = 0;
                 i < static_cast<int>(summaries.size());
                 i++) {
                if (summaries[i].locationId == r.locationId) {
                    index = i;
                    break;
                }
            }

            if (index == -1) {
                ProblemSummary p{};

                p.locationId = r.locationId;
                p.reviewCount = 0;
                p.verifiedCount = 0;
                p.severityTotal = 0;
                p.priorityScore = 0;

                summaries.push_back(p);
                index = static_cast<int>(summaries.size()) - 1;
            }

            summaries[index].reviewCount++;

            if (r.verified) {
                summaries[index].verifiedCount++;
                summaries[index].severityTotal += r.severity;
            }
        }

        if (summaries.empty()) {
            cout << "\nNo reviews available.\n";
            return;
        }

        MaxHeap maxHeap;

        for (ProblemSummary& p : summaries) {
            if (p.verifiedCount > 0) {
                p.priorityScore =
                    p.severityTotal +
                    2 * (p.verifiedCount - 1);
            }

            maxHeap.push(p);
        }

        cout << "\n======= PRIORITY RANKING =======\n";

        while (!maxHeap.empty()) {
            ProblemSummary p = maxHeap.pop();

            cout << "\nLocation ID: " << p.locationId;
            cout << "\nTotal reviews: " << p.reviewCount;
            cout << "\nVerified reviews: " << p.verifiedCount;
            cout << "\nPriority score: " << p.priorityScore;

            if (p.verifiedCount == 0)
                cout << "\nStatus: Awaiting verification";
            else if (p.priorityScore >= 10)
                cout << "\nStatus: High priority";
            else if (p.priorityScore >= 5)
                cout << "\nStatus: Needs attention";
            else
                cout << "\nStatus: Monitor";

            cout << "\n-----------------------------\n";
        }
    }
};

#endif
