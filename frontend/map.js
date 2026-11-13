
const mapLocations = {
    1:  { name: "Main Entrance", floor: 0, x: 100, y: 390, type: "entrance" },
    2:  { name: "West Entrance", floor: 0, x: 100, y: 190, type: "entrance" },
    3:  { name: "Central Atrium", floor: 0, x: 520, y: 390, type: "corridor" },
    4:  { name: "West Wing Junction", floor: 0, x: 280, y: 190, type: "corridor" },
    5:  { name: "East Wing Junction", floor: 0, x: 850, y: 390, type: "corridor" },
    6:  { name: "Elevator A", floor: 0, x: 520, y: 530, type: "elevator" },
    7:  { name: "Elevator B", floor: 0, x: 850, y: 530, type: "elevator" },
    8:  { name: "Staircase A", floor: 0, x: 280, y: 530, type: "stairs" },
    9:  { name: "Staircase B", floor: 0, x: 1050, y: 190, type: "stairs" },
    10: { name: "Staircase C", floor: 0, x: 1050, y: 530, type: "stairs" },
    11: { name: "Reception", floor: 0, x: 210, y: 390, type: "room" },
    12: { name: "Admissions Office", floor: 0, x: 350, y: 100, type: "room" },
    13: { name: "Lecture Hall 01", floor: 0, x: 520, y: 100, type: "room" },
    14: { name: "Library", floor: 0, x: 850, y: 100, type: "room" },
    15: { name: "Accessible Washroom", floor: 0, x: 700, y: 530, type: "room" },
    16: { name: "Student Lounge", floor: 0, x: 700, y: 260, type: "room" },

    17: { name: "First Floor Atrium", floor: 1, x: 520, y: 390, type: "corridor" },
    18: { name: "Elevator A - F1", floor: 1, x: 520, y: 530, type: "elevator" },
    19: { name: "Elevator B - F1", floor: 1, x: 850, y: 530, type: "elevator" },
    20: { name: "Staircase A - F1", floor: 1, x: 280, y: 530, type: "stairs" },
    21: { name: "Staircase B - F1", floor: 1, x: 1050, y: 190, type: "stairs" },
    22: { name: "Staircase C - F1", floor: 1, x: 1050, y: 530, type: "stairs" },
    23: { name: "West Wing Corridor", floor: 1, x: 280, y: 190, type: "corridor" },
    24: { name: "East Wing Corridor", floor: 1, x: 850, y: 390, type: "corridor" },
    25: { name: "Computer Laboratory", floor: 1, x: 350, y: 100, type: "room" },
    26: { name: "Research Laboratory", floor: 1, x: 520, y: 100, type: "room" },
    27: { name: "Seminar Room", floor: 1, x: 850, y: 100, type: "room" },
    28: { name: "Study Commons", floor: 1, x: 180, y: 300, type: "room" },
    29: { name: "Accessible Washroom F1", floor: 1, x: 700, y: 530, type: "room" },
    30: { name: "Faculty Offices", floor: 1, x: 700, y: 260, type: "room" },

    31: { name: "Second Floor Atrium", floor: 2, x: 520, y: 390, type: "corridor" },
    32: { name: "Elevator A - F2", floor: 2, x: 520, y: 530, type: "elevator" },
    33: { name: "Elevator B - F2", floor: 2, x: 850, y: 530, type: "elevator" },
    34: { name: "Staircase A - F2", floor: 2, x: 280, y: 530, type: "stairs" },
    35: { name: "Staircase B - F2", floor: 2, x: 1050, y: 190, type: "stairs" },
    36: { name: "Staircase C - F2", floor: 2, x: 1050, y: 530, type: "stairs" },
    37: { name: "West Wing Corridor F2", floor: 2, x: 280, y: 190, type: "corridor" },
    38: { name: "East Wing Corridor F2", floor: 2, x: 850, y: 390, type: "corridor" },
    39: { name: "Research Centre", floor: 2, x: 350, y: 100, type: "room" },
    40: { name: "Lecture Hall 201", floor: 2, x: 520, y: 100, type: "room" },
    41: { name: "Examination Hall", floor: 2, x: 850, y: 100, type: "room" },
    42: { name: "Quiet Study Area", floor: 2, x: 180, y: 300, type: "room" },
    43: { name: "Accessible Washroom F2", floor: 2, x: 700, y: 530, type: "room" },
    44: { name: "Conference Room", floor: 2, x: 700, y: 260, type: "room" }
};

const mapConnections = [];
let edgeId = 1;

function connect(a, b, type = "corridor", distance = 100) {
    mapConnections.push({
        id: edgeId++,
        from: a,
        to: b,
        type,
        distance
    });
}

// Ground-floor corridor network
// Direct left-turn entrance route to reception
connect(1, 11, "corridor", 20);

// Other ground-floor connections
connect(1, 4, "corridor", 45);
connect(2, 4, "corridor", 90);
connect(4, 3, "corridor", 110);
connect(3, 5, "corridor", 100);
connect(4, 8, "corridor", 100);
connect(8, 6, "corridor", 100);
connect(6, 3, "corridor", 100);
connect(3, 7, "corridor", 100);
connect(7, 5, "corridor", 90);
connect(5, 9, "corridor", 100);
connect(9, 14, "corridor", 100);
connect(5, 10, "corridor", 100);
connect(10, 7, "corridor", 100);

// Ground-floor facilities
connect(4, 11, "corridor", 25);
connect(4, 12, "corridor", 60);
connect(3, 13, "corridor", 60);
connect(5, 14, "corridor", 60);
connect(6, 15, "corridor", 60);
connect(3, 16, "corridor", 60);

// First-floor vertical connections
connect(6, 18, "elevator", 30);
connect(7, 19, "elevator", 30);
connect(8, 20, "stairs", 30);
connect(9, 21, "stairs", 30);
connect(10, 22, "stairs", 30);

// First-floor corridor network
connect(18, 17, "corridor", 100);
connect(19, 17, "corridor", 100);
connect(20, 23, "corridor", 100);
connect(23, 17, "corridor", 110);
connect(17, 24, "corridor", 100);
connect(24, 19, "corridor", 90);
connect(21, 24, "corridor", 100);
connect(22, 19, "corridor", 100);

// First-floor facilities
connect(23, 25, "corridor", 60);
connect(17, 26, "corridor", 60);
connect(24, 27, "corridor", 60);
connect(23, 28, "corridor", 60);
connect(19, 29, "corridor", 60);
connect(17, 30, "corridor", 60);

// Second-floor vertical connections
connect(18, 32, "elevator", 30);
connect(19, 33, "elevator", 30);
connect(20, 34, "stairs", 30);
connect(21, 35, "stairs", 30);
connect(22, 36, "stairs", 30);

// Second-floor corridor network
connect(32, 31, "corridor", 100);
connect(33, 31, "corridor", 100);
connect(34, 37, "corridor", 100);
connect(37, 31, "corridor", 110);
connect(31, 38, "corridor", 100);
connect(38, 33, "corridor", 90);
connect(35, 38, "corridor", 100);
connect(36, 33, "corridor", 100);

// Second-floor facilities
connect(37, 39, "corridor", 60);
connect(31, 40, "corridor", 60);
connect(38, 41, "corridor", 60);
connect(37, 42, "corridor", 60);
connect(33, 43, "corridor", 60);
connect(31, 44, "corridor", 60);

let currentMapRoute = [];
let activeMapFloor = 0;
let selectedRouteType = "accessible";
let availableRoutes = [];

function getNeighbours(id, mobility) {
    return mapConnections.filter(edge => {
        if (edge.from !== id && edge.to !== id) return false;
        if (mobility === "wheelchair" && edge.type === "stairs") return false;
        if (mobility === "limited" && edge.type === "stairs") return false;
        return true;
    }).map(edge => ({
        id: edge.from === id ? edge.to : edge.from,
        edge
    }));
}

function findPath(start, end, mobility, preference) {
    const distances = {};
    const previous = {};
    const visited = new Set();

    Object.keys(mapLocations).forEach(id => {
        distances[id] = Infinity;
    });

    distances[start] = 0;

    while (true) {
        let current = null;
        let best = Infinity;

        for (const id of Object.keys(distances)) {
            if (!visited.has(Number(id)) && distances[id] < best) {
                best = distances[id];
                current = Number(id);
            }
        }

        if (current === null || current === end) break;
        visited.add(current);

        for (const next of getNeighbours(current, mobility)) {
            const edge = next.edge;
            let cost = edge.distance;

            if (preference === "accessible" && edge.type === "stairs") {
                cost += 100000;
            }

            if (preference === "fewest" && edge.type === "stairs") {
                cost += 35;
            }

            if (preference === "accessible" && edge.type === "elevator") {
                cost += 5;
            }

            const candidate = distances[current] + cost;

            if (candidate < distances[next.id]) {
                distances[next.id] = candidate;
                previous[next.id] = current;
            }
        }
    }

    if (distances[end] === Infinity) return null;

    const path = [];
    let current = end;

    while (current !== undefined) {
        path.unshift(current);
        current = previous[current];
    }

    return path[0] === start ? path : null;
}

function getRouteDistance(route) {
    let total = 0;

    for (let i = 0; i < route.length - 1; i++) {
        const edge = mapConnections.find(e =>
            (e.from === route[i] && e.to === route[i + 1]) ||
            (e.to === route[i] && e.from === route[i + 1])
        );
        if (edge) total += edge.distance;
    }

    return total;
}

function populateMapSelectors() {
    const start = document.getElementById("map-start");
    const destination = document.getElementById("map-destination");
    if (!start || !destination) return;

    const oldStart = start.value || "1";
    const oldDestination = destination.value || "13";

    const options = Object.entries(mapLocations).map(([id, loc]) =>
        `<option value="${id}">Floor ${loc.floor} — ${loc.name}</option>`
    ).join("");

    start.innerHTML = options;
    destination.innerHTML = options;

    start.value = mapLocations[oldStart] ? oldStart : "1";
    destination.value = mapLocations[oldDestination] ? oldDestination : "13";
}

function drawRoom(location) {
    if (location.type !== "room") return "";

    const width = 150;
    const height = 75;
    const x = location.x - width / 2;
    const y = location.y - height / 2;

    return `
        <g class="new-map-room">
            <rect x="${x}" y="${y}" width="${width}" height="${height}" rx="9"/>
            <text x="${location.x}" y="${location.y - 4}">${location.name}</text>
            <text x="${location.x}" y="${location.y + 17}" class="room-small">FACILITY</text>
        </g>
    `;
}

function drawFloorEdges(floor) {
    return mapConnections.map(edge => {
        const a = mapLocations[edge.from];
        const b = mapLocations[edge.to];

        if (a.floor !== floor || b.floor !== floor) return "";

        const isRoute = currentMapRoute.some((id, index) =>
            index < currentMapRoute.length - 1 &&
            ((id === edge.from && currentMapRoute[index + 1] === edge.to) ||
             (id === edge.to && currentMapRoute[index + 1] === edge.from))
        );

        const middleX = (a.x + b.x) / 2;
        const middleY = (a.y + b.y) / 2;
        const bend = Math.abs(a.x - b.x) > Math.abs(a.y - b.y)
            ? `${middleX},${a.y} ${middleX},${b.y}`
            : `${a.x},${middleY} ${b.x},${middleY}`;

        return `
            <polyline
                points="${a.x},${a.y} ${bend} ${b.x},${b.y}"
                class="${isRoute ? "new-route-edge" : "new-map-edge"}"/>
        `;
    }).join("");
}

function drawMapNode(id) {
    const loc = mapLocations[id];
    const selected = currentMapRoute.includes(Number(id));
    const symbol = {
        entrance: "IN",
        corridor: "↔",
        elevator: "E",
        stairs: "S",
        room: "R"
    }[loc.type];

    return `
        <g class="new-map-node ${selected ? "selected" : ""}"
           data-location="${id}" tabindex="0" role="button">
            <circle cx="${loc.x}" cy="${loc.y}" r="17"/>
            <text x="${loc.x}" y="${loc.y + 4}" class="new-node-symbol">${symbol}</text>
            <text x="${loc.x}" y="${loc.y + 34}" class="new-node-label">${loc.name}</text>
        </g>
    `;
}

function renderFloorPlan(floor) {
    const floorNames = ["GROUND FLOOR", "FIRST FLOOR", "SECOND FLOOR"];
    const locations = Object.keys(mapLocations)
        .filter(id => mapLocations[id].floor === floor);

    let svg = `
        <svg viewBox="0 0 1200 700" role="img"
             aria-label="${floorNames[floor]} indoor map">
            <rect width="1200" height="700" rx="15" class="new-map-bg"/>

            <text x="40" y="48" class="new-map-title">${floorNames[floor]}</text>
            <text x="1160" y="48" text-anchor="end" class="new-map-subtitle">
                UNIVERSITY ACADEMIC COMPLEX · DEMO PLAN
            </text>

            <path d="M 80 390 L 520 390 L 850 390 L 1100 390"
                  class="new-corridor"/>
            <path d="M 280 150 L 280 530"
                  class="new-corridor"/>
            <path d="M 520 150 L 520 530"
                  class="new-corridor"/>
            <path d="M 850 150 L 850 530"
                  class="new-corridor"/>
            <path d="M 280 190 L 520 390 L 850 390"
                  class="new-corridor-branch"/>
            <path d="M 850 390 L 1050 190"
                  class="new-corridor-branch"/>
            <path d="M 520 390 L 700 260"
                  class="new-corridor-branch"/>

            <text x="520" y="355" class="new-corridor-label">CENTRAL ATRIUM WALKWAY</text>
            <text x="280" y="165" class="new-wing-label">WEST WING</text>
            <text x="850" y="165" class="new-wing-label">EAST WING</text>
    `;

    locations.forEach(id => {
        svg += drawRoom(mapLocations[id]);
    });

    svg += drawFloorEdges(floor);

    locations.forEach(id => {
        svg += drawMapNode(id);
    });

    svg += `</svg>`;
    return svg;
}

function renderIndoorMap() {
    const container = document.getElementById("indoor-map");
    if (!container) return;

    container.innerHTML = `
        <div class="floor-switcher">
            ${[0, 1, 2].map(floor => `
                <button type="button" data-floor="${floor}"
                    class="${activeMapFloor === floor ? "active" : ""}">
                    ${["Ground Floor", "First Floor", "Second Floor"][floor]}
                </button>
            `).join("")}
        </div>

        <div class="floor-plan-canvas new-floor-canvas">
            ${renderFloorPlan(activeMapFloor)}
        </div>

        <div class="map-key">
            <span>🟢 Entrance</span>
            <span>🔵 Corridor</span>
            <span>🟣 Elevator</span>
            <span>🟠 Staircase</span>
            <span>🟩 Selected route</span>
        </div>
    `;

    container.querySelectorAll("[data-floor]").forEach(button => {
        button.addEventListener("click", () => {
            activeMapFloor = Number(button.dataset.floor);
            renderIndoorMap();
        });
    });

    container.querySelectorAll(".new-map-node").forEach(node => {
        const choose = () => {
            const destination = document.getElementById("map-destination");
            destination.value = node.dataset.location;
            calculateAndShowMapRoute();
        };

        node.addEventListener("click", choose);
        node.addEventListener("keydown", event => {
            if (event.key === "Enter" || event.key === " ") {
                event.preventDefault();
                choose();
            }
        });
    });
}

function calculateAndShowMapRoute() {
    const start = Number(document.getElementById("map-start")?.value);
    const destination = Number(document.getElementById("map-destination")?.value);
    const mobility = document.getElementById("map-mobility")?.value || "general";
    const result = document.getElementById("map-route-result");

    if (!mapLocations[start] || !mapLocations[destination] || !result) return;

    if (start === destination) {
        currentMapRoute = [start];
        activeMapFloor = mapLocations[start].floor;
        renderIndoorMap();
        result.textContent = "You are already at your selected destination.";
        return;
    }

    const routeTypes = [
        { id: "shortest", label: "Shortest Route" },
        { id: "fewest", label: "Fewer Stairs Route" },
        { id: "accessible", label: "Accessible Route" }
    ];

    availableRoutes = routeTypes.map(type => ({
        ...type,
        path: findPath(start, destination, mobility, type.id)
    })).filter(route => route.path);

    const unique = [];
    const signatures = new Set();

    availableRoutes.forEach(route => {
        const signature = route.path.join("-");
        if (!signatures.has(signature)) {
            signatures.add(signature);
            unique.push(route);
        }
    });

    availableRoutes = unique;

    if (!availableRoutes.length) {
        currentMapRoute = [];
        renderIndoorMap();
        result.textContent =
            "No route is available for this mobility profile in the sample building.";
        return;
    }

    const selected = availableRoutes.find(route =>
        route.id === selectedRouteType
    ) || availableRoutes[0];

    selectedRouteType = selected.id;
    currentMapRoute = selected.path;
    activeMapFloor = mapLocations[destination].floor;
    renderIndoorMap();

    result.innerHTML = `
        <strong>Available route options</strong>
        <div class="new-route-options">
            ${availableRoutes.map(route => `
                <button type="button"
                    class="${route.id === selectedRouteType ? "active" : ""}"
                    data-route="${route.id}">
                    ${route.label}
                    <small>${route.path.length - 1} connections</small>
                </button>
            `).join("")}
        </div>
        <div class="route-path">
            ${currentMapRoute.map(id => mapLocations[id].name).join(" → ")}
        </div>
        <small>Approximate graph distance: ${getRouteDistance(currentMapRoute)} units.
        This is demonstration data, not measured building distance.</small>
    `;

    result.querySelectorAll("[data-route]").forEach(button => {
        button.addEventListener("click", () => {
            selectedRouteType = button.dataset.route;
            calculateAndShowMapRoute();
        });
    });
}

document.addEventListener("DOMContentLoaded", () => {
    populateMapSelectors();
    renderIndoorMap();

    document.getElementById("show-map-route")
        ?.addEventListener("click", calculateAndShowMapRoute);

    document.getElementById("map-start")
        ?.addEventListener("change", calculateAndShowMapRoute);

    document.getElementById("map-destination")
        ?.addEventListener("change", calculateAndShowMapRoute);

    document.getElementById("map-mobility")
        ?.addEventListener("change", calculateAndShowMapRoute);

    calculateAndShowMapRoute();
});
