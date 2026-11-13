
const mapLocations = {
    1: { name: "Main Entrance", floor: 0, x: 100, y: 280, type: "entrance" },
    2: { name: "Main Corridor", floor: 0, x: 300, y: 280, type: "corridor" },
    3: { name: "Elevator", floor: 0, x: 650, y: 280, type: "elevator" },
    4: { name: "Room 101", floor: 0, x: 460, y: 135, type: "room" },
    5: { name: "Staircase", floor: 0, x: 800, y: 280, type: "stairs" },
    6: { name: "First Floor Corridor", floor: 1, x: 300, y: 280, type: "corridor" },
    7: { name: "Room 201", floor: 1, x: 500, y: 135, type: "room" },
    8: { name: "Emergency Exit", floor: 1, x: 800, y: 280, type: "exit" }
};

const mapConnections = [
    { from: 1, to: 2 },
    { from: 2, to: 3 },
    { from: 2, to: 4 },
    { from: 3, to: 6, lift: true },
    { from: 2, to: 5, stairs: true },
    { from: 5, to: 6, stairs: true },
    { from: 6, to: 7 },
    { from: 7, to: 8 },
    { from: 2, to: 8 }
];

let currentMapRoute = [];
let activeMapFloor = 0;

function findIndoorRoute(start, destination, mobility) {
    const queue = [[start]];
    const visited = new Set([start]);

    while (queue.length) {
        const path = queue.shift();
        const current = path[path.length - 1];

        if (current === destination) return path;

        for (const edge of mapConnections) {
            let next = null;

            if (edge.from === current) next = edge.to;
            else if (edge.to === current) next = edge.from;

            if (next === null || visited.has(next)) continue;

            if (edge.stairs && mobility !== "general") continue;

            visited.add(next);
            queue.push([...path, next]);
        }
    }

    return null;
}


function drawRoom(x, y, width, height, label, roomId = "") {
    return `
        <g class="floor-room">
            <rect x="${x}" y="${y}" width="${width}" height="${height}"
                  rx="3"
                  fill="#f8fafc"
                  stroke="#8297aa"
                  stroke-width="3"/>
            <text x="${x + width / 2}" y="${y + height / 2 - 5}"
                  class="room-label" fill="#38536d">${label}</text>
            ${roomId ? `<text x="${x + width / 2}" y="${y + height / 2 + 17}"
                  class="room-number" fill="#8293a4">${roomId}</text>` : ""}
        </g>
    `;
}


function drawLocation(id, selectedFloor) {
    const location = mapLocations[id];
    const isSelected = currentMapRoute.includes(id);
    const isStart = currentMapRoute[0] === id;
    const isEnd = currentMapRoute[currentMapRoute.length - 1] === id;

    const symbols = {
        entrance: "IN",
        corridor: "↔",
        elevator: "E",
        room: "R",
        stairs: "S",
        exit: "EXIT"
    };

    const marker = isStart ? "START" : isEnd ? "DEST" : symbols[location.type];

    return `
        <g class="map-node ${isSelected ? "route-node" : ""}"
           data-location="${id}" tabindex="0" role="button"
           aria-label="Select ${location.name}">
            <circle cx="${location.x}" cy="${location.y}" r="19"
                    class="location-circle ${location.type}"/>
            <text x="${location.x}" y="${location.y + 5}"
                  class="location-symbol">${marker}</text>
            <text x="${location.x}" y="${location.y + 39}"
                  class="location-label">${location.name}</text>
        </g>
    `;
}


function drawRouteOnFloor(floor) {
    let output = "";

    for (let i = 0; i < currentMapRoute.length - 1; i++) {
        const a = mapLocations[currentMapRoute[i]];
        const b = mapLocations[currentMapRoute[i + 1]];

        if (!a || !b || a.floor !== floor || b.floor !== floor) {
            continue;
        }

        const startY = a.type === "room" ? 218 : a.y;
        const endY = b.type === "room" ? 218 : b.y;

        const points = [
            `${a.x},${startY}`,
            `${a.x},280`,
            `${b.x},280`,
            `${b.x},${endY}`
        ].join(" ");

        output += `
            <polyline
                points="${points}"
                fill="none"
                stroke="#00a99d"
                stroke-width="12"
                stroke-linecap="round"
                stroke-linejoin="round"
                opacity="1"/>
        `;
    }

    return output;
}


function renderFloorPlan(floor) {
    const isGround = floor === 0;

    let svg = `
   <svg viewBox="0 0 900 510"
     xmlns="http://www.w3.org/2000/svg"
     role="img"
     aria-label="${isGround ? "Ground" : "First"} floor building plan"
     style="background:#ffffff; display:block;">

     <rect x="15" y="15" width="870" height="480"
      rx="8" fill="#ffffff"
      stroke="#d7e1eb" stroke-width="2"/>

        <text x="45" y="48" class="plan-heading">
            ${isGround ? "GROUND FLOOR PLAN" : "FIRST FLOOR PLAN"}
        </text>

        <text x="855" y="48" text-anchor="end" class="plan-level">
            ${isGround ? "LEVEL 0" : "LEVEL 1"}
        </text>

        <!-- Main corridor -->
   <rect x="55" y="235" width="790" height="90"
      fill="#e9f2f8"
      stroke="#8297aa"
      stroke-width="3"/>
        <text x="450" y="270" class="corridor-title">
            ${isGround ? "MAIN CORRIDOR" : "FIRST FLOOR CORRIDOR"}
        </text>
        <text x="450" y="295" class="corridor-subtitle">
            WALKWAY
        </text>
    `;

    if (isGround) {
        svg += `
            ${drawRoom(55, 85, 155, 125, "RECEPTION", "R-001")}
            ${drawRoom(245, 85, 155, 125, "CLASSROOM", "R-102")}
            ${drawRoom(420, 85, 155, 125, "ROOM 101", "R-101")}
            ${drawRoom(595, 85, 145, 125, "LIBRARY", "R-103")}
            ${drawRoom(55, 350, 155, 105, "WAITING AREA", "R-004")}
            ${drawRoom(245, 350, 155, 105, "WASHROOM", "R-005")}
            ${drawRoom(420, 350, 155, 105, "STUDENT AREA", "R-006")}

            <rect x="745" y="225" width="100" height="110"
      fill="#eee8fc"
      stroke="#9274c8"
      stroke-width="3"/>
            <text x="795" y="260" class="service-label">LIFT</text>
            <text x="795" y="285" class="service-symbol">↕</text>

           <rect x="755" y="350" width="90" height="105"
      fill="#fff1d9"
      stroke="#d99b43"
      stroke-width="3"/>
            <text x="800" y="390" class="service-label">STAIRS</text>
            <text x="800" y="420" class="service-symbol">▥</text>
        `;
    } else {
        svg += `
            ${drawRoom(55, 85, 155, 125, "CLASSROOM", "R-202")}
            ${drawRoom(245, 85, 155, 125, "LABORATORY", "R-203")}
            ${drawRoom(420, 85, 155, 125, "ROOM 201", "R-201")}
            ${drawRoom(595, 85, 145, 125, "LIBRARY", "R-204")}
            ${drawRoom(55, 350, 155, 105, "STUDY AREA", "R-205")}
            ${drawRoom(245, 350, 155, 105, "WASHROOM", "R-206")}
            ${drawRoom(420, 350, 155, 105, "COMMON AREA", "R-207")}

           <rect x="745" y="225" width="100" height="110"
      fill="#eee8fc"
      stroke="#9274c8"
      stroke-width="3"/>
            <text x="795" y="260" class="service-label">LIFT</text>
            <text x="795" y="285" class="service-symbol">↕</text>

          <rect x="755" y="350" width="90" height="105"
      fill="#fff1d9"
      stroke="#d99b43"
      stroke-width="3"/>
            <text x="800" y="390" class="service-label">STAIRS</text>
            <text x="800" y="420" class="service-symbol">▥</text>

            <text x="450" y="475" class="exit-note">EMERGENCY EXIT →</text>
        `;
    }

    const visibleLocations = Object.keys(mapLocations)
        .filter(id => mapLocations[id].floor === floor);

    visibleLocations.forEach(id => {
        svg += drawLocation(Number(id), floor);
    });

      svg += drawRouteOnFloor(floor);
    svg += `</svg>`;
    return svg;
}

function renderIndoorMap() {
    const container = document.getElementById("indoor-map");
    if (!container) return;

    container.innerHTML = `
        <div class="floor-switcher">
            <button type="button" data-floor="0"
                class="${activeMapFloor === 0 ? "active" : ""}">
                Ground Floor
            </button>
            <button type="button" data-floor="1"
                class="${activeMapFloor === 1 ? "active" : ""}">
                First Floor
            </button>
        </div>
        <div class="floor-plan-canvas" style="background:#ffffff !important;">
            ${renderFloorPlan(activeMapFloor)}
        </div>
        <div class="map-key">
            <span><i class="key-dot entrance-key"></i> Entrance</span>
            <span><i class="key-dot room-key"></i> Room</span>
            <span><i class="key-dot lift-key"></i> Elevator</span>
            <span><i class="key-dot stairs-key"></i> Stairs</span>
            <span><i class="key-dot route-key"></i> Selected route</span>
        </div>
    `;

    container.querySelectorAll("[data-floor]").forEach(button => {
        button.addEventListener("click", () => {
            activeMapFloor = Number(button.dataset.floor);
            renderIndoorMap();
        });
    });

    container.querySelectorAll(".map-node").forEach(node => {
        const selectLocation = () => {
            document.getElementById("map-destination").value =
                node.dataset.location;
            calculateAndShowMapRoute();
        };

        node.addEventListener("click", selectLocation);
        node.addEventListener("keydown", event => {
            if (event.key === "Enter" || event.key === " ") {
                event.preventDefault();
                selectLocation();
            }
        });
    });
}

function calculateAndShowMapRoute() {
    const start = Number(document.getElementById("map-start").value);
    const destination = Number(document.getElementById("map-destination").value);
    const mobility = document.getElementById("map-mobility").value;
    const result = document.getElementById("map-route-result");

    if (start === destination) {
        currentMapRoute = [start];
        activeMapFloor = mapLocations[start].floor;
        renderIndoorMap();
        result.textContent = "You are already at your destination.";
        return;
    }

    const route = findIndoorRoute(start, destination, mobility);

    if (!route) {
        currentMapRoute = [];
        renderIndoorMap();
        result.textContent = "No route found for this mobility profile in the sample building.";
        return;
    }

    currentMapRoute = route;
    activeMapFloor = mapLocations[destination].floor;
    renderIndoorMap();

    const names = route.map(id => mapLocations[id].name);
    result.innerHTML = `
        <strong>Route found</strong>
        <div class="route-path">${names.join(" → ")}</div>
        <span>${route.length} locations along this sample route</span>
    `;
}

document.addEventListener("DOMContentLoaded", () => {
    renderIndoorMap();

    document.getElementById("show-map-route")
        ?.addEventListener("click", calculateAndShowMapRoute);

    document.getElementById("map-start")
        ?.addEventListener("change", calculateAndShowMapRoute);

    document.getElementById("map-destination")
        ?.addEventListener("change", calculateAndShowMapRoute);

    document.getElementById("map-mobility")
        ?.addEventListener("change", calculateAndShowMapRoute);
        // Automatically display the default accessible route
    calculateAndShowMapRoute();
});
