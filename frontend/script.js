
/* =========================================
   ACCESSWAY - FRONTEND FUNCTIONALITY
   Demonstration version using sample data
========================================= */

// =========================================
// 1. PAGE NAVIGATION
// =========================================

const pageTitles = {
    dashboard: "Dashboard",
    navigation: "Building Navigation",
    reviews: "Accessibility Reviews",
    emergency: "Emergency & Simulation",
    volunteers: "Volunteer Assistance"
};

function showPage(pageId) {
    const pages = document.querySelectorAll(".page");
    const links = document.querySelectorAll(".nav-link");

    pages.forEach(page => {
        page.classList.toggle("active", page.id === pageId);
    });

    links.forEach(link => {
        link.classList.toggle(
            "active",
            link.dataset.page === pageId
        );
    });

    const title = pageTitles[pageId] || "Dashboard";

    document.getElementById("page-heading").textContent = title;
    document.getElementById("breadcrumb-page").textContent = title;

    window.scrollTo({ top: 0, behavior: "smooth" });
}

document.querySelectorAll("[data-page]").forEach(button => {
    button.addEventListener("click", () => {
        showPage(button.dataset.page);
    });
});


// =========================================
// 2. SAMPLE BUILDING GRAPH
// =========================================

const locations = [
    { id: 1, name: "Main Entrance" },
    { id: 2, name: "Main Corridor" },
    { id: 3, name: "Elevator" },
    { id: 4, name: "Room 101" },
    { id: 5, name: "Staircase" },
    { id: 6, name: "First Floor Corridor" },
    { id: 7, name: "Room 201" },
    { id: 8, name: "Emergency Exit" }
];

const connections = [
    { id: 1, from: 1, to: 2, stairs: false },
    { id: 2, from: 2, to: 3, stairs: false },
    { id: 3, from: 3, to: 6, stairs: false },
    { id: 4, from: 6, to: 7, stairs: false },
    { id: 5, from: 2, to: 5, stairs: true },
    { id: 6, from: 5, to: 6, stairs: true },
    { id: 7, from: 2, to: 8, stairs: false }
];


// =========================================
// 3. GRAPH BFS
// =========================================

function findRoute(startId, endId, wheelchair, blocked = new Set(),
                   disabled = new Set()) {

    const queue = [[startId]];
    const visited = new Set([startId]);

    while (queue.length > 0) {
        const path = queue.shift();
        const current = path[path.length - 1];

        if (current === endId) {
            return path;
        }

        for (const connection of connections) {
            if (connection.from !== current) continue;
            if (blocked.has(connection.id)) continue;
            if (disabled.has(connection.from) ||
                disabled.has(connection.to)) continue;

            if (wheelchair && connection.stairs) continue;

            if (!visited.has(connection.to)) {
                visited.add(connection.to);
                queue.push([...path, connection.to]);
            }
        }
    }

    return null;
}

function countReachable(startId, wheelchair,
                        blocked = new Set(),
                        disabled = new Set()) {

    const queue = [startId];
    const visited = new Set([startId]);

    while (queue.length > 0) {
        const current = queue.shift();

        for (const connection of connections) {
            if (connection.from !== current) continue;
            if (blocked.has(connection.id)) continue;
            if (disabled.has(connection.from) ||
                disabled.has(connection.to)) continue;

            if (wheelchair && connection.stairs) continue;

            if (!visited.has(connection.to)) {
                visited.add(connection.to);
                queue.push(connection.to);
            }
        }
    }

    return visited.size;
}


// =========================================
// 4. BUILDING NAVIGATION
// =========================================

const routeForm = document.getElementById("route-form");

routeForm.addEventListener("submit", function(event) {
    event.preventDefault();

    const building = document.getElementById("building-select").value;
    const startName = document.getElementById("start-location").value.trim();
    const destinationName = document.getElementById("destination").value.trim();
    const mobility = document.getElementById("mobility").value;

    const result = document.getElementById("route-result");

    if (building !== "University Campus") {
        result.textContent =
            "Navigation for this building is sample-only and is not yet configured. Try University Campus.";
        return;
    }

    const start = locations.find(
        location => location.name.toLowerCase() === startName.toLowerCase()
    );

    const destination = locations.find(
        location => location.name.toLowerCase() === destinationName.toLowerCase()
    );

    if (!start || !destination) {
        result.textContent =
            "For this demonstration, use an exact location name from the sample building, such as Main Entrance or Room 201.";
        return;
    }

    const wheelchair = ["wheelchair", "crutches", "limited"].includes(mobility);

    const route = findRoute(start.id, destination.id, wheelchair);

    if (!route) {
        result.textContent =
            "No route was found in the sample graph for these locations and preferences.";
        return;
    }

    const routeNames = route.map(id =>
        locations.find(location => location.id === id).name
    );

    result.innerHTML = `
        <strong>Demo route found</strong><br>
        ${routeNames.map(escapeHTML).join(" → ")}
        <br><br>
        <strong>Number of locations:</strong> ${route.length}
        <br>
        <small>Sample graph result only. Verify actual building conditions before travelling.</small>
    `;
});


// =========================================
// 5. REVIEW DATA
// =========================================

const sampleReviews = [
    {
        id: 1, building: "University Campus",
        locationId: 5, location: "Staircase",
        category: "Steep ramp", severity: 5,
        description: "Ramp is difficult for wheelchair users.",
        verified: true
    },
    {
        id: 2, building: "University Campus",
        locationId: 3, location: "Elevator",
        category: "Elevator failure", severity: 4,
        description: "Elevator stopped working repeatedly.",
        verified: true
    },
    {
        id: 3, building: "University Campus",
        locationId: 5, location: "Staircase",
        category: "Difficult ramp access", severity: 3,
        description: "Ramp entrance is difficult to access.",
        verified: true
    },
    {
        id: 4, building: "University Campus",
        locationId: 2, location: "Main Corridor",
        category: "Blocked corridor", severity: 5,
        description: "Corridor is frequently blocked.",
        verified: true
    },
    {
        id: 5, building: "University Campus",
        locationId: 3, location: "Elevator",
        category: "Frequent breakdown", severity: 5,
        description: "Elevator frequently becomes unavailable.",
        verified: true
    },
    {
        id: 6, building: "University Campus",
        locationId: 7, location: "Room 201",
        category: "Door accessibility", severity: 4,
        description: "Door is difficult to open.",
        verified: false
    }
];

let reviews = sampleReviews;

try {
    const savedReviews = localStorage.getItem("accesswayReviews");

    if (savedReviews) {
        reviews = JSON.parse(savedReviews);
    }
} catch (error) {
    reviews = sampleReviews;
}

function saveReviews() {
    try {
        localStorage.setItem("accesswayReviews", JSON.stringify(reviews));
    } catch (error) {
        console.warn("Browser storage is unavailable.");
    }
}


// =========================================
// 6. SAFE HTML TEXT
// =========================================

function escapeHTML(value) {
    return String(value).replace(/[&<>"']/g, character => ({
        "&": "&amp;",
        "<": "&lt;",
        ">": "&gt;",
        '"': "&quot;",
        "'": "&#39;"
    })[character]);
}


// =========================================
// 7. DISPLAY REVIEWS
// =========================================

function renderReviews() {
    const list = document.getElementById("review-list");

    if (reviews.length === 0) {
        list.textContent = "No reviews submitted yet.";
    } else {
        list.innerHTML = reviews.slice().reverse().map(review => {
            const level = review.severity >= 4
                ? "high"
                : review.severity === 3 ? "medium" : "low";

            const status = review.verified
                ? "Verified"
                : "Awaiting verification";

            return `
                <div class="review-item">
                    <div>
                        <strong>${escapeHTML(review.category)}</strong>
                        <p>${escapeHTML(review.building)} ·
                           ${escapeHTML(review.location)}</p>
                        <p>Severity: ${review.severity}/5 · ${status}</p>
                    </div>
                    <span class="severity ${level}">
                        ${review.severity >= 4 ? "High" :
                          review.severity === 3 ? "Medium" : "Low"}
                    </span>
                </div>
            `;
        }).join("");
    }

    document.getElementById("review-count").textContent = reviews.length;

    document.getElementById("issue-count").textContent =
        reviews.filter(review => review.verified).length;

    renderPriorities();
}


// =========================================
// 8. SUBMIT REVIEW
// =========================================

document.getElementById("review-form").addEventListener(
    "submit",
    function(event) {
        event.preventDefault();

        const building = document.getElementById("review-building").value;
        const location = document.getElementById("review-location").value.trim();
        const category = document.getElementById("review-category").value;
        const severity = Number(
            document.getElementById("review-severity").value
        );
        const description =
            document.getElementById("review-description").value.trim();

        const knownLocation = locations.find(
            item => item.name.toLowerCase() === location.toLowerCase()
        );

        const newReview = {
            id: reviews.length
                ? Math.max(...reviews.map(review => review.id)) + 1
                : 1,
            building,
            locationId: knownLocation ? knownLocation.id : 0,
            location,
            category,
            severity,
            description,
            verified: false
        };

        reviews.push(newReview);
        saveReviews();
        renderReviews();

        alert("Review submitted successfully! It is awaiting verification.");

        this.reset();
    }
);


// =========================================
// 9. PRIORITY RANKING USING MAX HEAP
// =========================================

function generatePriorities() {
    const summaries = [];

    reviews.forEach(review => {
        let summary = summaries.find(
            item => item.locationId === review.locationId &&
                    item.building === review.building
        );

        if (!summary) {
            summary = {
                locationId: review.locationId,
                building: review.building,
                location: review.location,
                reviewCount: 0,
                verifiedCount: 0,
                severityTotal: 0,
                priorityScore: 0
            };

            summaries.push(summary);
        }

        summary.reviewCount++;

        if (review.verified) {
            summary.verifiedCount++;
            summary.severityTotal += review.severity;
        }
    });

    summaries.forEach(summary => {
        if (summary.verifiedCount > 0) {
            summary.priorityScore =
                summary.severityTotal +
                2 * (summary.verifiedCount - 1);
        }
    });

    // Custom max heap
    const heap = [];

    function higherPriority(a, b) {
        if (a.priorityScore !== b.priorityScore) {
            return a.priorityScore > b.priorityScore;
        }

        return a.locationId < b.locationId;
    }

    function push(item) {
        heap.push(item);

        let index = heap.length - 1;

        while (index > 0) {
            const parent = Math.floor((index - 1) / 2);

            if (!higherPriority(heap[index], heap[parent])) break;

            [heap[index], heap[parent]] =
                [heap[parent], heap[index]];

            index = parent;
        }
    }

    function pop() {
        const result = heap[0];
        const last = heap.pop();

        if (heap.length > 0) {
            heap[0] = last;

            let index = 0;

            while (true) {
                const left = 2 * index + 1;
                const right = 2 * index + 2;
                let largest = index;

                if (left < heap.length &&
                    higherPriority(heap[left], heap[largest])) {
                    largest = left;
                }

                if (right < heap.length &&
                    higherPriority(heap[right], heap[largest])) {
                    largest = right;
                }

                if (largest === index) break;

                [heap[index], heap[largest]] =
                    [heap[largest], heap[index]];

                index = largest;
            }
        }

        return result;
    }

    summaries.forEach(push);

    const result = [];

    while (heap.length > 0) {
        result.push(pop());
    }

    return result;
}


// =========================================
// 10. DISPLAY PRIORITY RANKING
// =========================================

function renderPriorities() {
    const container = document.querySelector(
        "#reviews .content-card:last-child"
    );

    if (!container) return;

    const priorities = generatePriorities();

    let html = "<h3>Accessibility Improvement Priorities</h3>";

    if (priorities.length === 0) {
        html += "<p>No review data available.</p>";
    }

    priorities.forEach(item => {
        const status = item.verifiedCount === 0
            ? "Awaiting verification"
            : item.priorityScore >= 10
                ? "High priority"
                : item.priorityScore >= 5
                    ? "Needs attention"
                    : "Monitor";

        html += `
            <div class="priority-item">
                <div>
                    <strong>${escapeHTML(item.location)}</strong>
                    <p>${item.reviewCount} reviews · ${status}</p>
                </div>
                <span class="priority-score">
                    ${item.priorityScore}
                </span>
            </div>
        `;
    });

    container.innerHTML = html;
}


// =========================================
// 11. WHAT-IF SIMULATION
// =========================================

let blockedConnections = new Set();
let disabledFacilities = new Set();

const simulationForm = document.getElementById("simulation-form");

simulationForm.addEventListener("submit", function(event) {
    event.preventDefault();

    const type = document.getElementById("scenario-type").value;
    const id = Number(document.getElementById("connection-id").value);

    if (type !== "elevator" &&
        !connections.some(connection => connection.id === id)) {
        document.getElementById("simulation-result").textContent =
            "Please enter a valid connection ID (1–7).";
        return;
    }

    if (type === "block") {
        blockedConnections.add(id);
    }

    if (type === "restore") {
        blockedConnections.delete(id);
    }

    if (type === "elevator") {
        disabledFacilities.add(3);
    }

    const before = countReachable(1, true);
    const after = countReachable(
        1, true, blockedConnections, disabledFacilities
    );

    const difference = after - before;

    let message = "";

    if (type === "block") {
        message = `Connection ${id} blocked in the demo scenario.`;
    } else if (type === "restore") {
        message = `Connection ${id} restored in the demo scenario.`;
    } else {
        message = "Elevator disabled in the demo scenario.";
    }

    const result = document.getElementById("simulation-result");

    result.innerHTML = `
        <strong>Simulation completed</strong><br>
        ${escapeHTML(message)}
        <br><br>
        <strong>Before:</strong> ${before} reachable locations
        <br>
        <strong>After:</strong> ${after} reachable locations
        <br>
        <strong>Difference:</strong> ${difference}
        <br><br>
        ${after < before
            ? "Accessibility has decreased."
            : after > before
                ? "Accessibility has improved."
                : "No change in reachability."}
    `;
});


// =========================================
// 12. RESET SIMULATION
// =========================================

document.getElementById("reset-simulation").addEventListener(
    "click",
    function() {
        blockedConnections.clear();
        disabledFacilities.clear();

        const reachable = countReachable(1, true);

        document.getElementById("simulation-result").innerHTML = `
            <strong>Scenario reset successfully.</strong>
            <br>
            Reachable locations restored to ${reachable}.
        `;
    }
);


// =========================================
// 13. VOLUNTEER ASSISTANCE
// =========================================

document.getElementById("help-form").addEventListener(
    "submit",
    function(event) {
        event.preventDefault();

        const building = document.getElementById("help-building").value;
        const location = document.getElementById("help-location").value;
        const type = document.getElementById("help-type").value;

        const result = document.getElementById("help-result");

        result.innerHTML = `
            <strong>Demo request recorded!</strong>
            <br>
            Building: ${escapeHTML(building)}
            <br>
            Location: ${escapeHTML(location)}
            <br>
            Assistance: ${escapeHTML(type)}
            <br><br>
            This is a frontend demonstration only. No volunteer has been notified.
        `;

        this.reset();
    }
);


// =========================================
// 14. INITIALIZE DASHBOARD
// =========================================

renderReviews();

document.getElementById("building-count").textContent = "3";
document.getElementById("volunteer-count").textContent = "12";

showPage("dashboard");

console.log("AccessWay frontend initialized successfully.");
