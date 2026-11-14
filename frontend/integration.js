
/* AccessWay frontend-to-C++ API integration
   This file is loaded by integrated.html only.
   The original index.html remains untouched. */

(() => {
  "use strict";

  const API_BASE = "https://accessway-jojd.onrender.com";
  const BUILDING_ID = 1;

  const nodeToId = new Map();
  const idToNode = new Map();
  const edgeByPair = new Map();

  let readyPromise = null;
  let backendReady = false;

  // Convert frontend mobility options to C++ API values.
  function getMobilityProfile(value) {
    const profiles = {
      wheel: "wheelchair",
      crutch: "crutches",
      limited: "limited_mobility",
      hand: "hand_disability",
      gen: "general"
    };

    return profiles[value] || "general";
  }

  // Convert frontend map node types to C++ LocationType values.
  function getLocationType(node) {
    switch (node.type) {
      case "entrance": return 0;
      case "room":
      case "hall":
      case "lobby": return 1;
      case "corridor": return 2;
      case "elev": return 3;
      case "stair": return 4;
      case "ramp": return 5;
      case "exit": return 6;
      case "refuge": return 7;
      case "wash": return 8;
      default: return 9;
    }
  }

  // Send JSON data to the C++ backend.
  async function apiPost(endpoint, data) {
    const response = await fetch(API_BASE + endpoint, {
      method: "POST",
      headers: {
        "Content-Type": "application/json"
      },
      body: JSON.stringify(data)
    });

    const result = await response.json().catch(() => ({}));

    if (!response.ok) {
      throw new Error(
        result.error || endpoint + " returned HTTP " + response.status
      );
    }

    return result;
  }

  // Load the frontend campus map into the C++ backend.
  async function initializeBackend() {
    if (readyPromise) {
      return readyPromise;
    }

    readyPromise = (async () => {
      const nodes = [...N.values()];

      nodes.forEach((node, index) => {
        const numericId = index + 1;

        nodeToId.set(node.id, numericId);
        idToNode.set(numericId, node.id);
      });

      const locations = nodes.map(node => ({
        locationId: nodeToId.get(node.id),
        buildingId: BUILDING_ID,
        locationName: node.label || node.id,
        floor: Number.isFinite(node.f) ? node.f : 0,
        type: getLocationType(node)
      }));

      const connections = E.map((edge, index) => {
        edgeByPair.set(
          [edge.a, edge.b].sort().join("|"),
          edge
        );

        return {
          connectionId: index + 1,
          buildingId: BUILDING_ID,

          fromLocationId: nodeToId.get(edge.a),
          toLocationId: nodeToId.get(edge.b),

          distance: Math.max(1, Number(edge.w) || 1),

          width: edge.cond === "grass" ? 0.8 : 1.5,

          slope: edge.cond === "ramp" ? 0.05 : 0,

          hasStairs: edge.t === "stair",
          hasRamp: edge.cond === "ramp",
          hasElevator: edge.t === "elev",

          automaticDoor: true,

          available: edge.cond !== "damaged",
          emergencyApproved: edge.cond !== "damaged"
        };
      });

      await apiPost("/building", {
        buildingId: BUILDING_ID,
        locations,
        connections
      });

      backendReady = true;

      console.info(
        "AccessWay C++ backend connected successfully.",
        locations.length,
        "locations;",
        connections.length,
        "connections."
      );

      return true;
    })().catch(error => {
      backendReady = false;

      console.error(
        "Backend initialization failed. Existing local frontend remains available.",
        error
      );

      return false;
    });

    return readyPromise;
  }

  // Keep the original JavaScript route function as a fallback.
  const originalFindRoutes = window.findRoutes;

  // Use the C++ backend for route searches when it is available.
  window.findRoutes = async function () {
    const from =
      typeof pickValue === "function"
        ? pickValue("route-from")
        : "";

    const to =
      typeof pickValue === "function"
        ? pickValue("route-to")
        : "";

    const profile =
      document.querySelector("#route-profile")?.value || "gen";

    if (!from || !to || from === to) {
      return originalFindRoutes();
    }

    const initialized =
      backendReady || await initializeBackend();

    if (
      !initialized ||
      !nodeToId.has(from) ||
      !nodeToId.has(to)
    ) {
      return originalFindRoutes();
    }

    try {
      const result = await apiPost("/routes", {
        buildingId: BUILDING_ID,
        fromLocationId: nodeToId.get(from),
        toLocationId: nodeToId.get(to),
        mobilityProfile: getMobilityProfile(profile)
      });

      if (
        !result.found ||
        !Array.isArray(result.path) ||
        result.path.length < 2
      ) {
        S.opts = [];
        S.searched = true;
        renderRouteResult();

        toast(
          result.message || "The backend could not find a route.",
          true
        );

        return;
      }

      // Convert C++ numeric location IDs back to frontend map IDs.
      const path = result.path.map(id => idToNode.get(id));

      if (path.some(id => !id)) {
        throw new Error("Backend returned an unknown map location.");
      }

      // Match each returned route segment to the frontend map edge.
      const routeEdges = [];

      for (let i = 0; i < path.length - 1; i++) {
        const edge = edgeByPair.get(
          [path[i], path[i + 1]].sort().join("|")
        );

        if (!edge) {
          throw new Error(
            "Could not match a backend route segment to the map."
          );
        }

        routeEdges.push(edge);
      }

      const metres = Math.round(
        Number(result.totalDistance) || 0
      );

      const walkingSpeeds = {
        wheel: 0.8,
        crutch: 0.9,
        limited: 1.0,
        hand: 1.2,
        gen: 1.3
      };

      const speed = walkingSpeeds[profile] || 1.3;

      const route = {
        path: path,
        es: routeEdges,
        m: metres,

        min: Math.max(
          1,
          Math.round(metres / speed / 60)
        ),

        stairs: routeEdges.filter(
          edge => edge.t === "stair"
        ).length,

        elev: routeEdges.filter(
          edge => edge.t === "elev"
        ).length,

        ramps: routeEdges.filter(
          edge => edge.cond === "ramp"
        ).length,

        turns: 0,
        conn: routeEdges.length
      };

      S.opts = [{
        title: "C++ backend route",
        r: route,
        sig: path.join()
      }];

      S.sel = 0;
      S.searched = true;

      renderRouteResult();
      focusRoute();

      document
        .querySelector("#directions-card")
        ?.scrollIntoView({
          behavior: "smooth",
          block: "start"
        });

    } catch (error) {
      console.error(
        "Backend route failed. Using the original local route.",
        error
      );

      toast(
        "Backend route unavailable. Showing the local route instead.",
        true
      );

      originalFindRoutes();
    }
  };

  // Submit accessibility reviews to the C++ backend too.
  // Capture the values before the original frontend handler clears them.
  document
    .querySelector("#review-form")
    ?.addEventListener("submit", event => {

      const form = event.currentTarget;

      const submitted = {
        building:
          form.querySelector("#review-building")?.value || "",

        location:
          form.querySelector("#review-location-text")
            ?.value.trim() || "",

        category:
          form.querySelector("#review-category")?.value ||
          "Accessibility issue",

        severity: Number(
          form.querySelector("#review-severity")?.value || 1
        ),

        description:
          form.querySelector("#review-description")
            ?.value.trim() || ""
      };

      queueMicrotask(async () => {
        try {
          const initialized =
            backendReady || await initializeBackend();

          if (!initialized) return;

          const block = BLOCKS.find(item =>
            submitted.building.startsWith(item.name)
          );

          const candidates = [...N.values()].filter(node =>
            block
              ? node.b === block.id
              : node.b === "-" && node.type === "place"
          );

          const search = submitted.location.toLowerCase();

          const match =
            candidates.find(node =>
              node.label &&
              node.label.toLowerCase().includes(search)
            ) ||
            candidates.find(node => node.type === "entrance") ||
            candidates.find(node => node.type === "lobby") ||
            candidates[0];

          if (!match) {
            throw new Error(
              "Could not match the report to a map location."
            );
          }

          await apiPost("/reviews", {
            id: Math.floor(Date.now() % 2000000000),
            buildingId: BUILDING_ID,
            locationId: nodeToId.get(match.id),
            severity: submitted.severity,
            category: submitted.category,

            description: [
              submitted.location,
              submitted.description
            ].filter(Boolean).join(": ")
          });

          console.info(
            "Review also submitted to the C++ backend."
          );

        } catch (error) {
          console.error(
            "Backend review submission failed. The existing frontend still handles the review.",
            error
          );
        }
      });

    }, true);

  // Submit visitor help requests to the C++ backend too.
  document
    .querySelector("#help-form")
    ?.addEventListener("submit", event => {

      const form = event.currentTarget;

      const selectedNode =
        form.querySelector("#help-loc-select")?.value || "og";

      const helpType =
        form.querySelector("#help-type")?.value || "general";

      queueMicrotask(async () => {
        try {
          const initialized =
            backendReady || await initializeBackend();

          if (!initialized) return;

          const locationId =
            nodeToId.get(selectedNode) ||
            nodeToId.get("og");

          if (!locationId) {
            throw new Error(
              "Could not match the help location to the map."
            );
          }

          await apiPost("/requests", {
            buildingId: BUILDING_ID,
            locationId: locationId,
            type: helpType
          });

          console.info(
            "Help request also submitted to the C++ backend."
          );

        } catch (error) {
          console.error(
            "Backend help request submission failed. The existing frontend still handles the request.",
            error
          );
        }
      });

    }, true);

  // Initialize the backend after the existing map has been created.
  initializeBackend();

})();
