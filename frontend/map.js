
// ================================================
// ACCESSWAY — UNIVERSITY ACADEMIC COMPLEX
// Three-floor indoor navigation demonstration
// ================================================

const mapLocations = {
    1:{name:"Main Entrance",floor:0,x:130,y:370,type:"entrance"},
    2:{name:"West Entrance",floor:0,x:130,y:190,type:"entrance"},
    3:{name:"Central Atrium",floor:0,x:600,y:370,type:"corridor"},
    4:{name:"West Wing Junction",floor:0,x:330,y:190,type:"corridor"},
    5:{name:"East Wing Junction",floor:0,x:900,y:370,type:"corridor"},
    6:{name:"Elevator A",floor:0,x:600,y:540,type:"elevator"},
    7:{name:"Elevator B",floor:0,x:900,y:540,type:"elevator"},
    8:{name:"Staircase A",floor:0,x:330,y:540,type:"stairs"},
    9:{name:"Staircase B",floor:0,x:1080,y:190,type:"stairs"},
    10:{name:"Staircase C",floor:0,x:1080,y:540,type:"stairs"},
    11:{name:"Reception",floor:0,x:260,y:270,type:"room"},
    12:{name:"Admissions Office",floor:0,x:400,y:190,type:"room"},
    13:{name:"Lecture Hall 01",floor:0,x:600,y:190,type:"room"},
    14:{name:"Library",floor:0,x:900,y:190,type:"room"},
    15:{name:"Accessible Washroom",floor:0,x:750,y:540,type:"room"},
    16:{name:"Student Lounge",floor:0,x:750,y:370,type:"room"},

    17:{name:"First Floor Atrium",floor:1,x:600,y:370,type:"corridor"},
    18:{name:"Elevator A - F1",floor:1,x:600,y:540,type:"elevator"},
    19:{name:"Elevator B - F1",floor:1,x:900,y:540,type:"elevator"},
    20:{name:"Staircase A - F1",floor:1,x:330,y:540,type:"stairs"},
    21:{name:"Staircase B - F1",floor:1,x:1080,y:190,type:"stairs"},
    22:{name:"Staircase C - F1",floor:1,x:1080,y:540,type:"stairs"},
    23:{name:"West Wing Corridor",floor:1,x:330,y:190,type:"corridor"},
    24:{name:"East Wing Corridor",floor:1,x:900,y:370,type:"corridor"},
    25:{name:"Computer Laboratory",floor:1,x:400,y:190,type:"room"},
    26:{name:"Research Laboratory",floor:1,x:600,y:190,type:"room"},
    27:{name:"Seminar Room",floor:1,x:900,y:190,type:"room"},
    28:{name:"Study Commons",floor:1,x:260,y:270,type:"room"},
    29:{name:"Accessible Washroom F1",floor:1,x:750,y:540,type:"room"},
    30:{name:"Faculty Offices",floor:1,x:750,y:370,type:"room"},

    31:{name:"Second Floor Atrium",floor:2,x:600,y:370,type:"corridor"},
    32:{name:"Elevator A - F2",floor:2,x:600,y:540,type:"elevator"},
    33:{name:"Elevator B - F2",floor:2,x:900,y:540,type:"elevator"},
    34:{name:"Staircase A - F2",floor:2,x:330,y:540,type:"stairs"},
    35:{name:"Staircase B - F2",floor:2,x:1080,y:190,type:"stairs"},
    36:{name:"Staircase C - F2",floor:2,x:1080,y:540,type:"stairs"},
    37:{name:"West Wing Corridor F2",floor:2,x:330,y:190,type:"corridor"},
    38:{name:"East Wing Corridor F2",floor:2,x:900,y:370,type:"corridor"},
    39:{name:"Research Centre",floor:2,x:400,y:190,type:"room"},
    40:{name:"Lecture Hall 201",floor:2,x:600,y:190,type:"room"},
    41:{name:"Examination Hall",floor:2,x:900,y:190,type:"room"},
    42:{name:"Quiet Study Area",floor:2,x:260,y:270,type:"room"},
    43:{name:"Accessible Washroom F2",floor:2,x:750,y:540,type:"room"},
    44:{name:"Conference Room",floor:2,x:750,y:370,type:"room"}
};

// Room rectangles are separate from corridor and doorway nodes.
// Each room has a doorway point connected to the corridor graph.
const roomLayouts = {
    11:{x:155,y:105,w:210,h:105,doorX:260,doorY:270},
    12:{x:350,y:55,w:170,h:100,doorX:400,doorY:190},
    13:{x:540,y:55,w:190,h:100,doorX:600,doorY:190},
    14:{x:820,y:55,w:180,h:100,doorX:900,doorY:190},
    15:{x:675,y:585,w:170,h:85,doorX:750,doorY:540},
    16:{x:675,y:245,w:170,h:85,doorX:750,doorY:370},

    25:{x:350,y:55,w:170,h:100,doorX:400,doorY:190},
    26:{x:540,y:55,w:190,h:100,doorX:600,doorY:190},
    27:{x:820,y:55,w:180,h:100,doorX:900,doorY:190},
    28:{x:155,y:105,w:210,h:105,doorX:260,doorY:270},
    29:{x:675,y:585,w:170,h:85,doorX:750,doorY:540},
    30:{x:675,y:245,w:170,h:85,doorX:750,doorY:370},

    39:{x:350,y:55,w:170,h:100,doorX:400,doorY:190},
    40:{x:540,y:55,w:190,h:100,doorX:600,doorY:190},
    41:{x:820,y:55,w:180,h:100,doorX:900,doorY:190},
    42:{x:155,y:105,w:210,h:105,doorX:260,doorY:270},
    43:{x:675,y:585,w:170,h:85,doorX:750,doorY:540},
    44:{x:675,y:245,w:170,h:85,doorX:750,doorY:370}
};

const mapConnections = [];
let edgeId = 1;

function connect(a,b,type="corridor",distance=100){
    mapConnections.push({id:edgeId++,from:a,to:b,type,distance});
}

// Ground floor — main route and alternative loops
connect(1,11,"corridor",25);
connect(1,4,"corridor",90);
connect(2,4,"corridor",90);
connect(4,3,"corridor",120);
connect(4,8,"corridor",100);
connect(8,6,"corridor",100);
connect(6,3,"corridor",100);
connect(3,5,"corridor",100);
connect(3,7,"corridor",100);
connect(7,5,"corridor",80);
connect(5,9,"corridor",100);
connect(9,14,"corridor",70);
connect(5,10,"corridor",100);
connect(10,7,"corridor",80);
connect(4,12,"corridor",65);
connect(3,13,"corridor",65);
connect(5,14,"corridor",65);
connect(6,15,"corridor",65);
connect(3,16,"corridor",65);

// First-floor vertical links and corridors
connect(6,18,"elevator",30);
connect(7,19,"elevator",30);
connect(8,20,"stairs",30);
connect(9,21,"stairs",30);
connect(10,22,"stairs",30);
connect(18,17,"corridor",100);
connect(19,17,"corridor",100);
connect(20,23,"corridor",100);
connect(23,17,"corridor",120);
connect(17,24,"corridor",100);
connect(24,19,"corridor",90);
connect(21,24,"corridor",100);
connect(22,19,"corridor",100);
connect(23,25,"corridor",65);
connect(17,26,"corridor",65);
connect(24,27,"corridor",65);
connect(23,28,"corridor",65);
connect(19,29,"corridor",65);
connect(17,30,"corridor",65);

// Second-floor vertical links and corridors
connect(18,32,"elevator",30);
connect(19,33,"elevator",30);
connect(20,34,"stairs",30);
connect(21,35,"stairs",30);
connect(22,36,"stairs",30);
connect(32,31,"corridor",100);
connect(33,31,"corridor",100);
connect(34,37,"corridor",100);
connect(37,31,"corridor",120);
connect(31,38,"corridor",100);
connect(38,33,"corridor",90);
connect(35,38,"corridor",100);
connect(36,33,"corridor",100);
connect(37,39,"corridor",65);
connect(31,40,"corridor",65);
connect(38,41,"corridor",65);
connect(37,42,"corridor",65);
connect(33,43,"corridor",65);
connect(31,44,"corridor",65);

let currentMapRoute = [];
let activeMapFloor = 0;
let selectedRouteType = "accessible";
let availableRoutes = [];

function getNeighbours(id,mobility){
    return mapConnections.filter(edge=>{
        if(edge.from!==id && edge.to!==id) return false;
        if((mobility==="wheelchair" || mobility==="limited") && edge.type==="stairs") return false;
        return true;
    }).map(edge=>({
        id:edge.from===id?edge.to:edge.from,
        edge
    }));
}

function findPath(start,end,mobility,preference){
    const dist={},previous={},visited=new Set();
    Object.keys(mapLocations).forEach(id=>dist[id]=Infinity);
    dist[start]=0;

    while(true){
        let current=null,best=Infinity;
        for(const id of Object.keys(dist)){
            if(!visited.has(Number(id)) && dist[id]<best){
                best=dist[id];
                current=Number(id);
            }
        }
        if(current===null || current===end) break;
        visited.add(current);

        for(const next of getNeighbours(current,mobility)){
            let cost=next.edge.distance;
            if(preference==="fewest" && next.edge.type==="stairs") cost+=80;
            if(preference==="accessible" && next.edge.type==="stairs") cost+=100000;
            if(preference==="accessible" && next.edge.type==="elevator") cost+=5;
            const candidate=dist[current]+cost;
            if(candidate<dist[next.id]){
                dist[next.id]=candidate;
                previous[next.id]=current;
            }
        }
    }

    if(dist[end]===Infinity) return null;
    const path=[];
    let current=end;
    while(current!==undefined){
        path.unshift(current);
        current=previous[current];
    }
    return path[0]===start?path:null;
}

function routeDistance(route){
    let total=0;
    for(let i=0;i<route.length-1;i++){
        const edge=mapConnections.find(e=>
            (e.from===route[i] && e.to===route[i+1]) ||
            (e.to===route[i] && e.from===route[i+1])
        );
        if(edge) total+=edge.distance;
    }
    return total;
}

function populateMapSelectors(){
    const start=document.getElementById("map-start");
    const destination=document.getElementById("map-destination");
    if(!start || !destination) return;

    const oldStart=start.value || "1";
    const oldDestination=destination.value || "11";

    const options=Object.entries(mapLocations).map(([id,loc])=>
        `<option value="${id}">Floor ${loc.floor} — ${loc.name}</option>`
    ).join("");

    start.innerHTML=options;
    destination.innerHTML=options;
    start.value=mapLocations[oldStart]?oldStart:"1";
    destination.value=mapLocations[oldDestination]?oldDestination:"11";
}

function drawCorridors(floor){
    const paths={
        0:[
            "M 130 370 H 330",
            "M 130 190 H 330 V 370 H 600 H 900",
            "M 330 370 V 540",
            "M 600 370 V 540",
            "M 900 370 V 540",
            "M 900 370 H 1080 V 190",
            "M 900 540 H 1080"
        ],
        1:[
            "M 330 190 V 370 H 600 H 900",
            "M 330 370 V 540",
            "M 600 370 V 540",
            "M 900 370 V 540",
            "M 900 370 H 1080 V 190",
            "M 900 540 H 1080"
        ],
        2:[
            "M 330 190 V 370 H 600 H 900",
            "M 330 370 V 540",
            "M 600 370 V 540",
            "M 900 370 V 540",
            "M 900 370 H 1080 V 190",
            "M 900 540 H 1080"
        ]
    };

    return paths[floor].map(d=>`
        <path d="${d}" class="architectural-corridor"/>
        <path d="${d}" class="corridor-centerline"/>
    `).join("");
}

function drawRooms(floor){
    return Object.entries(roomLayouts).map(([id,r])=>{
        if(mapLocations[id].floor!==floor) return "";
        return `
            <g class="architectural-room">
                <rect x="${r.x}" y="${r.y}" width="${r.w}" height="${r.h}" rx="7"/>
                <text x="${r.x+r.w/2}" y="${r.y+35}">${mapLocations[id].name}</text>
                <text x="${r.x+r.w/2}" y="${r.y+55}" class="room-subtitle">FACILITY</text>
                <path d="M ${r.doorX-13} ${r.doorY} H ${r.doorX+13}"
                      class="room-door"/>
            </g>
        `;
    }).join("");
}

function edgePoints(a,b){
    if(a.type==="room" || b.type==="room"){
        return `${a.x},${a.y} ${b.x},${b.y}`;
    }
    if(a.x===b.x || a.y===b.y){
        return `${a.x},${a.y} ${b.x},${b.y}`;
    }
    const mx=(a.x+b.x)/2;
    return `${a.x},${a.y} ${mx},${a.y} ${mx},${b.y} ${b.x},${b.y}`;
}

function drawEdges(floor){
    return mapConnections.map(edge=>{
        const a=mapLocations[edge.from],b=mapLocations[edge.to];
        if(a.floor!==floor || b.floor!==floor) return "";
        const active=currentMapRoute.some((id,i)=>
            i<currentMapRoute.length-1 &&
            ((id===edge.from && currentMapRoute[i+1]===edge.to) ||
             (id===edge.to && currentMapRoute[i+1]===edge.from))
        );
        return `<polyline points="${edgePoints(a,b)}"
            class="${active?"route-edge":"graph-edge"}"/>`;
    }).join("");
}

function drawNodes(floor){
    return Object.entries(mapLocations).map(([id,loc])=>{
        if(loc.floor!==floor) return "";
        const active=currentMapRoute.includes(Number(id));
        const symbol={
            entrance:"IN",corridor:"↔",elevator:"E",stairs:"S",room:"R"
        }[loc.type];
        return `
            <g class="architectural-node ${active?"route-node":""}"
               data-location="${id}" tabindex="0" role="button"
               aria-label="${loc.name}">
                <circle cx="${loc.x}" cy="${loc.y}" r="16"/>
                <text x="${loc.x}" y="${loc.y+4}" class="node-symbol">${symbol}</text>
                <text x="${loc.x}" y="${loc.y+32}" class="node-label">${loc.name}</text>
            </g>
        `;
    }).join("");
}

function renderFloorPlan(floor){
    const names=["GROUND FLOOR","FIRST FLOOR","SECOND FLOOR"];
    return `
        <svg viewBox="0 0 1200 700" role="img"
             aria-label="${names[floor]} building plan">
            <rect width="1200" height="700" rx="12" class="plan-background"/>
            <text x="40" y="45" class="plan-title">${names[floor]}</text>
            <text x="1160" y="45" text-anchor="end" class="plan-caption">
                UNIVERSITY ACADEMIC COMPLEX · SAMPLE PLAN
            </text>

            ${drawCorridors(floor)}
            ${drawEdges(floor)}
            ${drawRooms(floor)}
            ${drawNodes(floor)}

            <text x="330" y="165" class="wing-label">WEST WING</text>
            <text x="600" y="345" class="wing-label">CENTRAL WING</text>
            <text x="900" y="165" class="wing-label">EAST WING</text>
        </svg>
    `;
}

function renderIndoorMap(){
    const container=document.getElementById("indoor-map");
    if(!container) return;

    container.innerHTML=`
        <div class="floor-switcher">
            ${["Ground Floor","First Floor","Second Floor"].map((name,i)=>`
                <button type="button" data-floor="${i}"
                    class="${activeMapFloor===i?"active":""}">${name}</button>
            `).join("")}
        </div>
        <div class="floor-plan-canvas architectural-map">
            ${renderFloorPlan(activeMapFloor)}
        </div>
        <div class="map-key">
            <span>🟢 Entrance</span>
            <span>🔵 Corridor junction</span>
            <span>🟣 Elevator</span>
            <span>🟠 Staircase</span>
            <span>🟩 Selected route</span>
        </div>
    `;

    container.querySelectorAll("[data-floor]").forEach(button=>{
        button.addEventListener("click",()=>{
            activeMapFloor=Number(button.dataset.floor);
            renderIndoorMap();
        });
    });

    container.querySelectorAll(".architectural-node").forEach(node=>{
        const choose=()=>{
            const dest=document.getElementById("map-destination");
            if(dest) dest.value=node.dataset.location;
            calculateAndShowMapRoute();
        };
        node.addEventListener("click",choose);
        node.addEventListener("keydown",event=>{
            if(event.key==="Enter" || event.key===" "){
                event.preventDefault();
                choose();
            }
        });
    });
}

function calculateAndShowMapRoute(){
    const start=Number(document.getElementById("map-start")?.value);
    const destination=Number(document.getElementById("map-destination")?.value);
    const mobility=document.getElementById("map-mobility")?.value || "general";
    const result=document.getElementById("map-route-result");

    if(!mapLocations[start] || !mapLocations[destination] || !result) return;

    if(start===destination){
        currentMapRoute=[start];
        activeMapFloor=mapLocations[start].floor;
        renderIndoorMap();
        result.textContent="You are already at your destination.";
        return;
    }

    const types=[
        {id:"shortest",label:"Shortest Route"},
        {id:"fewest",label:"Fewer Stairs Route"},
        {id:"accessible",label:"Accessible Route"}
    ];

    const routes=types.map(type=>({
        ...type,
        path:findPath(start,destination,mobility,type.id)
    })).filter(route=>route.path);

    const unique=[];
    const signatures=new Set();
    routes.forEach(route=>{
        const key=route.path.join("-");
        if(!signatures.has(key)){
            signatures.add(key);
            unique.push(route);
        }
    });

    availableRoutes=unique;
    if(!availableRoutes.length){
        currentMapRoute=[];
        renderIndoorMap();
        result.textContent="No route found for this mobility profile.";
        return;
    }

    const selected=availableRoutes.find(r=>r.id===selectedRouteType)
        || availableRoutes[0];

    selectedRouteType=selected.id;
    currentMapRoute=selected.path;
    activeMapFloor=mapLocations[destination].floor;
    renderIndoorMap();

    result.innerHTML=`
        <strong>Route choices</strong>
        <div class="new-route-options">
            ${availableRoutes.map(route=>`
                <button type="button" data-route="${route.id}"
                    class="${route.id===selectedRouteType?"active":""}">
                    ${route.label}
                    <small>${route.path.length-1} connections</small>
                </button>
            `).join("")}
        </div>
        <div class="route-path">
            ${currentMapRoute.map(id=>mapLocations[id].name).join(" → ")}
        </div>
        <small>Illustrative graph distance: ${routeDistance(currentMapRoute)} units.</small>
    `;

    result.querySelectorAll("[data-route]").forEach(button=>{
        button.addEventListener("click",()=>{
            selectedRouteType=button.dataset.route;
            calculateAndShowMapRoute();
        });
    });
}

document.addEventListener("DOMContentLoaded",()=>{
    populateMapSelectors();
    renderIndoorMap();

    document.getElementById("show-map-route")
        ?.addEventListener("click",calculateAndShowMapRoute);
    document.getElementById("map-start")
        ?.addEventListener("change",calculateAndShowMapRoute);
    document.getElementById("map-destination")
        ?.addEventListener("change",calculateAndShowMapRoute);
    document.getElementById("map-mobility")
        ?.addEventListener("change",calculateAndShowMapRoute);

    calculateAndShowMapRoute();
});
