
const mapLocations = {
    1: { name: "Main Entrance", x: 90, y: 135, floor: 0, type: "entrance" },
    2: { name: "Main Corridor", x: 260, y: 135, floor: 0, type: "corridor" },
    3: { name: "Elevator", x: 430, y: 75, floor: 0, type: "elevator" },
    4: { name: "Room 101", x: 430, y: 195, floor: 0, type: "room" },
    5: { name: "Staircase", x: 260, y: 265, floor: 0, type: "stairs" },
    6: { name: "First Floor Corridor", x: 700, y: 135, floor: 1, type: "corridor" },
    7: { name: "Room 201", x: 870, y: 135, floor: 1, type: "room" },
    8: { name: "Emergency Exit", x: 870, y: 265, floor: 1, type: "exit" }
};

const mapConnections = [
    { from: 1, to: 2 },
    { from: 2, to: 3 },
    { from: 2, to: 4 },
    { from: 3, to: 6, accessible: true },
    { from: 2, to: 5, stairs: true },
    { from: 5, to: 6, stairs: true },
    { from: 6, to: 7 },
    { from: 7, to: 8 },
    { from: 2, to: 8 }
];

let currentMapRoute = [];

function findIndoorRoute(start, destination, mobility) {
    const queue = [[start]];
    const visited = new Set([start]);

    while (queue.length > 0) {
        const path = queue.shift();
        const current = path[path.length - 1];

        if (current === destination) {
            return path;
        }

        for (const edge of mapConnections) {
            let next = null;

            if (edge.from === current) next = edge.to;
            else if (edge.to === current) next = edge.from;

            if (next === null || visited.has(next)) continue;

            const restricted =
                edge.stairs &&
                mobility !== "general";

            if (restricted) continue;

            visited.add(next);
            queue.push([...path, next]);
        }
    }

    return null;
}

function renderIndoorMap() {
    const container = document.getElementById("indoor-map");

    const colors = {
        entrance: "#b9efd6",
        corridor: "#d5e8fa",
        room: "#ffffff",
        elevator: "#e6d5fa",
        stairs: "#ffe3b5",
        exit: "#ffd5d5"
    };

    let svg = `
    <svg viewBox="0 0 1000 350"
         xmlns="http://www.w3.org/2000/svg">

        <rect x="10" y="10" width="480" height="325"
              rx="12" fill="#f8fbff" stroke="#c7d5e3"/>

        <rect x="510" y="10" width="480" height="325"
              rx="12" fill="#f8fbff" stroke="#c7d5e3"/>

        <text x="25" y="40" class="floor-title">GROUND FLOOR</text>
        <text x="525" y="40" class="floor-title">FIRST FLOOR</text>
    `;

    for (const edge of mapConnections) {
        const a = mapLocations[edge.from];
        const b = mapLocations[edge.to];

        if (a.floor !== b.floor) continue;

        const onRoute = currentMapRoute.some((id, index) =>
            index < currentMapRoute.length - 1 &&
            ((id === edge.from &&
              currentMapRoute[index + 1] === edge.to) ||
             (id === edge.to &&
              currentMapRoute[index + 1] === edge.from))
        );

        svg += `
            <line
                x1="${a.x}" y1="${a.y}"
                x2="${b.x}" y2="${b.y}"
                class="map-edge ${edge.stairs ? "stairs" : ""} ${onRoute ? "route-edge" : ""}"
            />
        `;
    }

    for (const [id, location] of Object.entries(mapLocations)) {
        const selected = currentMapRoute.includes(Number(id));
        const isStart = currentMapRoute[0] === Number(id);
        const isEnd = currentMapRoute[currentMapRoute.length - 1] === Number(id);

        svg += `
            <g class="map-node ${selected ? "route-node" : ""}"
               data-location="${id}">

                <rect x="${location.x - 62}"
                      y="${location.y - 23}"
                      width="124"
                      height="46"
                      fill="${colors[location.type]}"/>

                <text x="${location.x}" y="${location.y + 5}">
                    ${isStart ? "START: " : ""}
                    ${isEnd ? "DEST: " : ""}
                    ${location.name}
                </text>
            </g>
        `;
    }

    svg += `
        <text x="500" y="315"
              text-anchor="middle"
              fill="#61748b"
              font-size="12">
            Cross-floor connection: elevator or stairs
        </text>
    </svg>`;

    container.innerHTML = svg;

    container.querySelectorAll(".map-node").forEach(node => {
        node.addEventListener("click", () => {
            document.getElementById("map-destination").value =
                node.dataset.location;

            calculateAndShowMapRoute();
        });
    });
}

function calculateAndShowMapRoute() {
    const start = Number(document.getElementById("map-start").value);
    const destination = Number(
        document.getElementById("map-destination").value
    );

    const mobility = document.getElementById("map-mobility").value;
    const result = document.getElementById("map-route-result");

    if (start === destination) {
        currentMapRoute = [start];
        renderIndoorMap();
        result.textContent =
            "Your starting point is already your destination.";
        return;
    }

    const route = findIndoorRoute(start, destination, mobility);

    if (!route) {
        currentMapRoute = [];
        renderIndoorMap();

        result.textContent =
            "No route was found for this mobility profile in the sample map. Please choose another destination or mobility profile.";

        return;
    }

    currentMapRoute = route;
    renderIndoorMap();

    const names = route.map(id => mapLocations[id].name);

    result.innerHTML = `
        <strong>Accessible route found!</strong><br>
        ${names.join(" → ")}<br>
        <strong>Locations along route:</strong> ${route.length}
    `;
}

document.addEventListener("DOMContentLoaded", () => {
    renderIndoorMap();

    document.getElementById("show-map-route")
        .addEventListener("click", calculateAndShowMapRoute);

    document.getElementById("map-start")
        .addEventListener("change", calculateAndShowMapRoute);

    document.getElementById("map-destination")
        .addEventListener("change", calculateAndShowMapRoute);

    document.getElementById("map-mobility")
        .addEventListener("change", calculateAndShowMapRoute);
});
