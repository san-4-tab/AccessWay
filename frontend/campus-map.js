
/* ==========================================
   ACCESSWAY - SAMPLE UNIVERSITY CAMPUS MAP
   Hybrid campus navigation demonstration
========================================== */

const campusLocations = {
    1: { name: "Main Gate", x: 450, y: 555, type: "gate" },
    2: { name: "Central Junction", x: 450, y: 410, type: "junction" },
    3: { name: "West Junction", x: 250, y: 410, type: "junction" },
    4: { name: "East Junction", x: 650, y: 410, type: "junction" },
    5: { name: "Block A - Academic", x: 250, y: 220, type: "building" },
    6: { name: "Block B - Library", x: 650, y: 220, type: "building" },
    7: { name: "Block C - Medical Centre", x: 90, y: 410, type: "building" },
    8: { name: "Block D - Cafeteria", x: 810, y: 410, type: "building" },
    9: { name: "Accessible Parking", x: 450, y: 490, type: "parking" }
};

const campusConnections = [
    { from: 1, to: 2, ramp: true, distance: 145 },
    { from: 2, to: 3, ramp: true, distance: 200 },
    { from: 2, to: 4, ramp: true, distance: 200 },
    { from: 3, to: 5, ramp: true, distance: 190 },
    { from: 4, to: 6, ramp: true, distance: 190 },
    { from: 3, to: 7, ramp: true, distance: 160 },
    { from: 4, to: 8, ramp: true, distance: 160 },
    { from: 9, to: 2, ramp: true, distance: 80 }
];

let campusRoute = [];

function findCampusPath(start, destination, mobility) {
    const queue = [[start]];
    const visited = new Set([start]);

    while (queue.length > 0) {
        const path = queue.shift();
        const current = path[path.length - 1];

        if (current === destination) return path;

        for (const edge of campusConnections) {
            let next = null;

            if (edge.from === current) next = edge.to;
            else if (edge.to === current) next = edge.from;

            if (next === null || visited.has(next)) continue;

            if (mobility !== "general" && !edge.ramp) continue;

            visited.add(next);
            queue.push([...path, next]);
        }
    }

    return null;
}

function renderCampusMap() {
    const container = document.getElementById("campus-map");
    if (!container) return;

    let svg = `
    <svg viewBox="0 0 900 640"
         xmlns="http://www.w3.org/2000/svg"
         role="img"
         aria-label="Illustrative university campus map">

        <rect width="900" height="640" rx="18" fill="#f4f7f3"/>

        <!-- Green campus areas -->
        <path d="M20 20 H880 V620 H20 Z"
              fill="#f2f7ef"/>

        <rect x="35" y="35" width="830" height="565"
              rx="20" fill="none"
              stroke="#d6e3d2" stroke-width="3"/>

        <!-- Gardens -->
        <rect x="350" y="65" width="200" height="75"
              rx="18" fill="#dcefd9"/>
        <text x="450" y="108" text-anchor="middle"
              class="campus-park-label">CENTRAL GARDEN</text>

        <rect x="350" y="465" width="200" height="55"
              rx="14" fill="#dcefd9"/>
        <text x="450" y="498" text-anchor="middle"
              class="campus-park-label">GREEN COURT</text>

        <!-- Campus pathways -->
        <g class="campus-base-paths">
            <path d="M450 555 L450 410 L250 410 L250 220"
                  class="campus-walkway"/>
            <path d="M450 410 L650 410 L650 220"
                  class="campus-walkway"/>
            <path d="M250 410 L90 410"
                  class="campus-walkway"/>
            <path d="M650 410 L810 410"
                  class="campus-walkway"/>
            <path d="M450 490 L450 410"
                  class="campus-walkway"/>
            <path d="M250 410 L250 330 L450 330 L650 330 L650 410"
                  class="campus-sidewalk"/>
        </g>

        <!-- Buildings -->
        <g class="campus-building">
            <rect x="135" y="145" width="230" height="125"
                  rx="13" fill="#dcecff" stroke="#6193c5" stroke-width="3"/>
            <text x="250" y="190" class="campus-building-icon">A</text>
            <text x="250" y="225" class="campus-building-name">BLOCK A</text>
            <text x="250" y="247" class="campus-building-type">ACADEMIC</text>
        </g>

        <g class="campus-building">
            <rect x="535" y="145" width="230" height="125"
                  rx="13" fill="#d9f3e8" stroke="#60a98a" stroke-width="3"/>
            <text x="650" y="190" class="campus-building-icon">B</text>
            <text x="650" y="225" class="campus-building-name">BLOCK B</text>
            <text x="650" y="247" class="campus-building-type">LIBRARY</text>
        </g>

        <g class="campus-building">
            <rect x="35" y="345" width="110" height="130"
                  rx="13" fill="#fce9d5" stroke="#d49a55" stroke-width="3"/>
            <text x="90" y="392" class="campus-building-icon">C</text>
            <text x="90" y="423" class="campus-building-name">BLOCK C</text>
            <text x="90" y="445" class="campus-building-type">MEDICAL</text>
        </g>

        <g class="campus-building">
            <rect x="755" y="345" width="110" height="130"
                  rx="13" fill="#eee5fc" stroke="#9a7cc7" stroke-width="3"/>
            <text x="810" y="392" class="campus-building-icon">D</text>
            <text x="810" y="423" class="campus-building-name">BLOCK D</text>
            <text x="810" y="445" class="campus-building-type">CAFETERIA</text>
        </g>

        <!-- Accessible ramps -->
        <g class="campus-ramp">
            <circle cx="250" cy="310" r="17"/>
            <text x="250" y="316">R</text>
            <circle cx="650" cy="310" r="17"/>
            <text x="650" y="316">R</text>
            <circle cx="450" cy="350" r="17"/>
            <text x="450" y="356">R</text>
        </g>

        <!-- Route is drawn here -->
        <g id="campus-route-layer"></g>

        <!-- Main gate -->
        <g class="campus-location-marker" data-campus-location="1">
            <circle cx="450" cy="555" r="19" class="campus-gate-marker"/>
            <text x="450" y="560" class="campus-marker-symbol">G</text>
            <text x="450" y="590" class="campus-marker-label">MAIN GATE</text>
        </g>

        <!-- Parking -->
        <g class="campus-location-marker" data-campus-location="9">
            <circle cx="450" cy="490" r="17" class="campus-parking-marker"/>
            <text x="450" y="496" class="campus-marker-symbol">P</text>
        </g>

        <!-- Junctions -->
        <g class="campus-junctions">
            <circle cx="450" cy="410" r="8"/>
            <circle cx="250" cy="410" r="8"/>
            <circle cx="650" cy="410" r="8"/>
        </g>

        <!-- Clickable buildings -->
        <g class="campus-location-marker" data-campus-location="5">
            <circle cx="250" cy="285" r="15" class="campus-building-marker"/>
            <text x="250" y="290" class="campus-marker-symbol">A</text>
        </g>

        <g class="campus-location-marker" data-campus-location="6">
            <circle cx="650" cy="285" r="15" class="campus-building-marker"/>
            <text x="650" y="290" class="campus-marker-symbol">B</text>
        </g>

        <g class="campus-location-marker" data-campus-location="7">
            <circle cx="90" cy="490" r="15" class="campus-building-marker"/>
            <text x="90" y="495" class="campus-marker-symbol">C</text>
        </g>

        <g class="campus-location-marker" data-campus-location="8">
            <circle cx="810" cy="490" r="15" class="campus-building-marker"/>
            <text x="810" y="495" class="campus-marker-symbol">D</text>
        </g>

        <text x="450" y="45" text-anchor="middle"
              class="campus-map-title">ACCESSWAY UNIVERSITY CAMPUS</text>
    </svg>`;

    container.innerHTML = svg;

    drawCampusRoute();

    container.querySelectorAll("[data-campus-location]").forEach(marker => {
        marker.addEventListener("click", () => {
            const id = marker.dataset.campusLocation;
            const destination = document.getElementById("campus-destination");

            if ([5, 6, 7, 8].includes(Number(id))) {
                destination.value = id;
            } else {
                document.getElementById("campus-start").value = id;
            }

            calculateCampusRoute();
        });
    });
}

function drawCampusRoute() {
    const layer = document.getElementById("campus-route-layer");
    if (!layer) return;

    if (campusRoute.length < 2) {
        layer.innerHTML = "";
        return;
    }

    const points = campusRoute.map(id => {
        const location = campusLocations[id];
        return `${location.x},${location.y}`;
    }).join(" ");

    layer.innerHTML = `
        <polyline points="${points}" class="campus-route-line"/>
        <circle cx="${campusLocations[campusRoute[0]].x}"
                cy="${campusLocations[campusRoute[0]].y}"
                r="12" class="campus-route-start"/>
        <circle cx="${campusLocations[campusRoute[campusRoute.length - 1]].x}"
                cy="${campusLocations[campusRoute[campusRoute.length - 1]].y}"
                r="12" class="campus-route-end"/>
    `;
}

function calculateCampusRoute() {
    const start = Number(document.getElementById("campus-start").value);
    const destination = Number(document.getElementById("campus-destination").value);
    const mobility = document.getElementById("campus-mobility").value;
    const result = document.getElementById("campus-route-result");

    const route = findCampusPath(start, destination, mobility);

    if (!route) {
        campusRoute = [];
        drawCampusRoute();
        result.textContent = "No route found in the sample campus network.";
        return;
    }

    campusRoute = route;
    drawCampusRoute();

    const routeNames = route.map(id => campusLocations[id].name);
    const distance = route.slice(0, -1).reduce((total, id, index) => {
        const nextId = route[index + 1];
        const edge = campusConnections.find(item =>
            (item.from === id && item.to === nextId) ||
            (item.from === nextId && item.to === id)
        );
        return total + (edge ? edge.distance : 0);
    }, 0);

    result.innerHTML = `
        <strong>Campus route found</strong>
        <div class="campus-route-path">${routeNames.join(" → ")}</div>
        <span>Sample route distance: ${distance} metres</span>
        <p>Illustrative campus data. Actual paths and ramp conditions must be verified.</p>
    `;
}

document.addEventListener("DOMContentLoaded", () => {
    renderCampusMap();

    document.getElementById("campus-find-route")
        ?.addEventListener("click", calculateCampusRoute);

    document.getElementById("campus-start")
        ?.addEventListener("change", calculateCampusRoute);

    document.getElementById("campus-destination")
        ?.addEventListener("change", calculateCampusRoute);

    document.getElementById("campus-mobility")
        ?.addEventListener("change", calculateCampusRoute);

    calculateCampusRoute();
});
