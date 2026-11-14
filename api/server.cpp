
#include "httplib.h"
#include "json.hpp"

#include "../EmergencyManager.h"
#include "../member3/ReviewManager.h"

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

using json = nlohmann::json;

static EmergencyManager emergency;
static ReviewManager reviews;
static int currentBuildingId = 1;

// Convert mobility profile text from the frontend.
static MobilityProfile getMobilityProfile(const std::string& value) {
    if (value == "wheelchair") return WHEELCHAIR;
    if (value == "crutches") return CRUTCHES;
    if (value == "hand_disability") return HAND_DISABILITY;
    if (value == "limited_mobility") return LIMITED_MOBILITY;
    return GENERAL;
}

static RequestStatus getRequestStatus(const std::string& value) {
    if (value == "accepted") return ACCEPTED;
    if (value == "in_progress") return IN_PROGRESS;
    if (value == "completed") return COMPLETED;
    if (value == "cancelled") return CANCELLED;
    return PENDING;
}

static json routeToJson(const RouteResult& route) {
    return {
        {"found", route.found},
        {"buildingId", route.buildingId},
        {"startLocationId", route.startLocationId},
        {"destinationLocationId", route.destinationLocationId},
        {"usedRefugeArea", route.usedRefugeArea},
        {"totalDistance", route.totalDistance},
        {"path", route.path},
        {"pathNames", route.pathNames},
        {"message", route.message}
    };
}

static json reviewToJson(const Review& review) {
    return {
        {"id", review.reviewId},
        {"buildingId", review.buildingId},
        {"locationId", review.locationId},
        {"severity", review.severity},
        {"category", review.category},
        {"description", review.description},
        {"verified", review.verified}
    };
}

static json requestToJson(const HelpRequest& request) {
    return {
        {"id", request.requestId},
        {"buildingId", request.buildingId},
        {"locationId", request.visitorLocationId},
        {"assignedVolunteerId", request.assignedVolunteerId},
        {"type", request.helpType},
        {"status", HelpRequestManager::statusToString(request.status)}
    };
}

static json volunteerToJson(const Volunteer& volunteer) {
    return {
        {"id", volunteer.volunteerId},
        {"buildingId", volunteer.buildingId},
        {"name", volunteer.volunteerName},
        {"assistanceType", volunteer.assistanceType},
        {"available", volunteer.available},
        {"locationId", volunteer.currentLocationId}
    };
}

int main() {
    httplib::Server server;

    // Allow browser frontend requests.
    server.set_default_headers({
        {"Access-Control-Allow-Origin", "*"},
        {"Access-Control-Allow-Methods", "GET, POST, OPTIONS"},
        {"Access-Control-Allow-Headers", "Content-Type"}
    });

    server.Options(R"(.*)", [](const httplib::Request&, httplib::Response& res) {
        res.status = 204;
    });

    // Health check.
    server.Get("/health", [](const httplib::Request&, httplib::Response& res) {
        res.set_content(
            R"({"status":"ok","service":"AccessWay API"})",
            "application/json"
        );
    });

    // Load or replace building graph data.
    // Expected JSON:
    // {
    //   "buildingId": 1,
    //   "locations": [
    //     {"locationId":1,"buildingId":1,"locationName":"Entrance",
    //      "floor":0,"type":0}
    //   ],
    //   "connections": [
    //     {"connectionId":1,"buildingId":1,"fromLocationId":1,
    //      "toLocationId":2,"distance":10,"width":2,"slope":0,
    //      "hasStairs":false,"hasRamp":true,"hasElevator":false,
    //      "automaticDoor":true,"available":true,"emergencyApproved":true}
    //   ]
    // }
    server.Post("/building", [](const httplib::Request& req, httplib::Response& res) {
        try {
            json body = json::parse(req.body);

            BuildingData data;
            currentBuildingId = body.value("buildingId", 1);

            for (const auto& item : body.at("locations")) {
                Location location{};
                location.locationId = item.at("locationId").get<int>();
                location.buildingId = item.value("buildingId", currentBuildingId);
                location.locationName = item.value("locationName", "");
                location.floor = item.value("floor", 0);
                location.type = static_cast<LocationType>(item.value("type", 0));
                data.locations.push_back(location);
            }

            for (const auto& item : body.at("connections")) {
                Connection connection{};
                connection.connectionId = item.at("connectionId").get<int>();
                connection.buildingId = item.value("buildingId", currentBuildingId);
                connection.fromLocationId = item.at("fromLocationId").get<int>();
                connection.toLocationId = item.at("toLocationId").get<int>();
                connection.distance = item.value("distance", 1.0f);
                connection.width = item.value("width", 1.0f);
                connection.slope = item.value("slope", 0.0f);
                connection.hasStairs = item.value("hasStairs", false);
                connection.hasRamp = item.value("hasRamp", false);
                connection.hasElevator = item.value("hasElevator", false);
                connection.automaticDoor = item.value("automaticDoor", false);
                connection.available = item.value("available", true);
                connection.emergencyApproved = item.value("emergencyApproved", true);
                data.connections.push_back(connection);
            }

            bool loaded = emergency.loadBuildingData(data);
            reviews.setBuildingData(data);

            if (!loaded) {
                res.status = 400;
                res.set_content(
                    R"({"error":"Could not load building data"})",
                    "application/json"
                );
                return;
            }

            res.set_content(
                json{{"success", true}, {"buildingId", currentBuildingId}}.dump(),
                "application/json"
            );
        }
        catch (const std::exception& e) {
            res.status = 400;
            res.set_content(
                json{{"error", e.what()}}.dump(),
                "application/json"
            );
        }
    });

    // Get reviews for a building.
    server.Get("/reviews", [](const httplib::Request& req, httplib::Response& res) {
        int buildingId = req.has_param("buildingId")
            ? std::stoi(req.get_param_value("buildingId"))
            : currentBuildingId;

        json result = json::array();
        for (const auto& review : reviews.getReviews(buildingId)) {
            result.push_back(reviewToJson(review));
        }

        res.set_content(result.dump(), "application/json");
    });

    // Submit a review.
    // Required: id, locationId, severity, category, description.
    server.Post("/reviews", [](const httplib::Request& req, httplib::Response& res) {
        try {
            json body = json::parse(req.body);

            Review review{};
            review.reviewId = body.value("id", 0);
            review.buildingId = body.value("buildingId", currentBuildingId);
            review.locationId = body.at("locationId").get<int>();
            review.severity = body.at("severity").get<int>();
            review.category = body.at("category").get<std::string>();
            review.description = body.at("description").get<std::string>();
            review.verified = false;

            if (review.reviewId <= 0 || review.locationId <= 0 ||
                review.severity < 1 || review.severity > 5 ||
                review.category.empty() || review.description.empty()) {
                res.status = 400;
                res.set_content(
                    R"({"error":"Invalid review details"})",
                    "application/json"
                );
                return;
            }

         const std::vector<Review> existingReviews =
    reviews.getReviews(review.buildingId);

if (std::any_of(
    existingReviews.begin(),
    existingReviews.end(),
    [&](const Review& existing) {
        return existing.reviewId == review.reviewId;
    })) {
                res.status = 409;
                res.set_content(
                    R"({"error":"Review ID already exists"})",
                    "application/json"
                );
                return;
            }

            reviews.addReview(review);

            res.set_content(
                json{{"success", true}, {"review", reviewToJson(review)}}.dump(),
                "application/json"
            );
        }
        catch (const std::exception& e) {
            res.status = 400;
            res.set_content(
                json{{"error", e.what()}}.dump(),
                "application/json"
            );
        }
    });

    // Verify a review.
    server.Post("/reviews/verify", [](const httplib::Request& req, httplib::Response& res) {
        try {
            json body = json::parse(req.body);
            int id = body.at("id").get<int>();

            bool success = reviews.verifyReview(id);
            res.status = success ? 200 : 404;
            res.set_content(
                json{{"success", success}}.dump(),
                "application/json"
            );
        }
        catch (const std::exception& e) {
            res.status = 400;
            res.set_content(
                json{{"error", e.what()}}.dump(),
                "application/json"
            );
        }
    });

    // Rank reviewed locations using the existing backend scoring formula.
    server.Get("/ranking", [](const httplib::Request& req, httplib::Response& res) {
        int buildingId = req.has_param("buildingId")
            ? std::stoi(req.get_param_value("buildingId"))
            : currentBuildingId;

        std::vector<Review> allReviews = reviews.getReviews(buildingId);
        std::vector<ProblemSummary> summaries;

        for (const auto& review : allReviews) {
            auto it = std::find_if(
                summaries.begin(), summaries.end(),
                [&](const ProblemSummary& item) {
                    return item.locationId == review.locationId;
                }
            );

            if (it == summaries.end()) {
                summaries.push_back({review.locationId, 0, 0, 0, 0});
                it = summaries.end() - 1;
            }

            it->reviewCount++;
            if (review.verified) {
                it->verifiedCount++;
                it->severityTotal += review.severity;
            }
        }

        MaxHeap heap;
        for (auto& item : summaries) {
            if (item.verifiedCount > 0) {
                item.priorityScore =
                    item.severityTotal + 2 * (item.verifiedCount - 1);
            }
            heap.push(item);
        }

        json result = json::array();
        while (!heap.empty()) {
            ProblemSummary item = heap.pop();
            std::string status = "Awaiting verification";

            if (item.verifiedCount > 0) {
                if (item.priorityScore >= 10) status = "High priority";
                else if (item.priorityScore >= 5) status = "Needs attention";
                else status = "Monitor";
            }

            result.push_back({
                {"locationId", item.locationId},
                {"reviewCount", item.reviewCount},
                {"verifiedCount", item.verifiedCount},
                {"severityTotal", item.severityTotal},
                {"priorityScore", item.priorityScore},
                {"status", status}
            });
        }

        res.set_content(result.dump(), "application/json");
    });

    // Create a visitor help request.
    server.Post("/requests", [](const httplib::Request& req, httplib::Response& res) {
        try {
            json body = json::parse(req.body);

            HelpRequest request{};
            request.requestId = body.value(
                "id",
                emergency.getHelpRequestManager().nextRequestId(currentBuildingId)
            );
            request.buildingId = body.value("buildingId", currentBuildingId);
            request.visitorLocationId = body.at("locationId").get<int>();
            request.assignedVolunteerId = 0;
            request.helpType = body.value("type", "general");
            request.status = PENDING;

            bool success = emergency.createHelpRequest(request);
            res.status = success ? 201 : 400;
            res.set_content(
                json{{"success", success}, {"request", requestToJson(request)}}.dump(),
                "application/json"
            );
        }
        catch (const std::exception& e) {
            res.status = 400;
            res.set_content(
                json{{"error", e.what()}}.dump(),
                "application/json"
            );
        }
    });

    // List help requests.
    server.Get("/requests", [](const httplib::Request& req, httplib::Response& res) {
        int buildingId = req.has_param("buildingId")
            ? std::stoi(req.get_param_value("buildingId"))
            : currentBuildingId;

        json result = json::array();
        for (const auto& request :
             emergency.getHelpRequestManager().getAllRequests(buildingId)) {
            result.push_back(requestToJson(request));
        }

        res.set_content(result.dump(), "application/json");
    });

    // Update help request status.
    server.Post("/requests/status", [](const httplib::Request& req, httplib::Response& res) {
        try {
            json body = json::parse(req.body);
            int id = body.at("id").get<int>();
            int buildingId = body.value("buildingId", currentBuildingId);
            RequestStatus status = getRequestStatus(body.value("status", "pending"));

            bool success = emergency.updateRequestStatus(buildingId, id, status);
            res.status = success ? 200 : 400;
            res.set_content(
                json{{"success", success}}.dump(),
                "application/json"
            );
        }
        catch (const std::exception& e) {
            res.status = 400;
            res.set_content(
                json{{"error", e.what()}}.dump(),
                "application/json"
            );
        }
    });

    // Register a volunteer.
    server.Post("/volunteers", [](const httplib::Request& req, httplib::Response& res) {
        try {
            json body = json::parse(req.body);

            Volunteer volunteer{};
            volunteer.volunteerId = body.value("id", 0);
            volunteer.buildingId = body.value("buildingId", currentBuildingId);
            volunteer.volunteerName = body.at("name").get<std::string>();
            volunteer.assistanceType = body.value("assistanceType", "general");
            volunteer.available = body.value("available", true);
            volunteer.currentLocationId = body.value("locationId", 0);

            bool success = emergency.registerVolunteer(volunteer);
            res.status = success ? 201 : 400;
            res.set_content(
                json{{"success", success}, {"volunteer", volunteerToJson(volunteer)}}.dump(),
                "application/json"
            );
        }
        catch (const std::exception& e) {
            res.status = 400;
            res.set_content(
                json{{"error", e.what()}}.dump(),
                "application/json"
            );
        }
    });

    // List volunteers.
    server.Get("/volunteers", [](const httplib::Request& req, httplib::Response& res) {
        int buildingId = req.has_param("buildingId")
            ? std::stoi(req.get_param_value("buildingId"))
            : currentBuildingId;

        json result = json::array();
        for (const auto& volunteer :
             emergency.getVolunteerManager().getAllVolunteers(buildingId)) {
            result.push_back(volunteerToJson(volunteer));
        }

        res.set_content(result.dump(), "application/json");
    });

    // Find a route between two location IDs.
    server.Post("/routes", [](const httplib::Request& req, httplib::Response& res) {
        try {
            json body = json::parse(req.body);
            int buildingId = body.value("buildingId", currentBuildingId);
            int from = body.at("fromLocationId").get<int>();
            int to = body.at("toLocationId").get<int>();
            MobilityProfile profile = getMobilityProfile(
                body.value("mobilityProfile", "general")
            );

            RouteResult route = emergency.getEvacuationManager().findRoute(
                buildingId, from, to, profile
            );

            res.set_content(routeToJson(route).dump(), "application/json");
        }
        catch (const std::exception& e) {
            res.status = 400;
            res.set_content(
                json{{"error", e.what()}}.dump(),
                "application/json"
            );
        }
    });

    std::cout << "AccessWay API starting on http://localhost:8080\n";

    if (!server.listen("0.0.0.0", 8080)) {
        std::cerr << "Could not start API server on port 8080\n";
        return 1;
    }

    return 0;
}
