# pathfinder

A shortest-path routing engine written in C, wrapped in a small Node/Express API
and a Leaflet web map. It loads an OpenStreetMap extract of Bucharest, builds a
weighted graph of the road network, and serves point-to-point routes computed
with Dijkstra's algorithm.

Click twice on the map — once for the start, once for the destination — and the
route is drawn along the roads, together with its total length.

## How it fits together

```
index.html  ──fetch──▶  server.js ──execFile──▶  pathfinder  ──reads──▶  map.json
 (Leaflet)              (Express)                 (C engine)
```

1. **`pathfinder`** (built from `main.c`) reads `map.json`, builds the graph and
   prints the route as a GeoJSON coordinate array on stdout.
2. **`server.js`** is a thin Express wrapper: it shells out to that binary and
   exposes it as `GET /api/route`.
3. **`index.html`** draws the map, collects the two clicks, calls the API and
   renders the polyline plus the distance readout.

## Requirements

| | |
|---|---|
| Compiler | gcc (developed against 13.3.0) |
| Node.js | 18 or newer — Express 5 needs it (developed against 22.23.0) |
| Map data | `map.json`, an Overpass API export (see below) |

## Build

From the project root:

```sh
gcc main.c Graph.c HashTable.c Heap.c List.c cJSON.c -lm -o pathfinder
```

`-lm` is required for `haversine()` in `main.c`. The output name matters: the
server runs `./pathfinder` relative to the project root.

## Running the engine directly

```sh
./pathfinder <startLat> <startLon> <destLat> <destLon>
```

For example:

```sh
./pathfinder 44.3888 26.1026 44.4200 26.1300
```

It prints a GeoJSON coordinate array — `[lon, lat]` pairs, destination first —
which is directly usable as the `coordinates` of a `LineString`:

```json
[
  [26.129955, 44.419983],
  [26.129920, 44.419921],
  ...
]
```

The API returns the same array, re-serialized compactly by `res.json`.

The coordinates you pass are snapped to the nearest node in the graph. The
engine exits `1` on a wrong argument count, a missing `map.json`, or a parse
failure, and prints nothing in those cases.

## Running the web app

```sh
npm install
node server.js
```

Then open <http://localhost:3000>. The server serves `index.html` as a static
file and answers the API on the same port.

### API

```
GET /api/route?startLat=<lat>&startLon=<lon>&destLat=<lat>&destLon=<lon>
```

| Status | Meaning |
|---|---|
| `200` | A coordinate array, as printed by the engine |
| `400` | One or more query parameters are missing |
| `500` | The engine failed, or its output was not valid JSON |

## Implementation

**Graph** (`Graph.c`) — adjacency lists, one per vertex, plus a parallel array
of node coordinates. Edges are stored in both directions, so the graph is
undirected.

**Hash table** (`HashTable.c`) — separate chaining, mapping the 64-bit OSM node
id to the small internal index used everywhere else. `initHashTable(V * 2)`
keeps the load factor around 0.5.

**Heap** (`Heap.c`) — a binary min-heap over `{vertex, cost}` pairs, doubling
its backing array when full. This is what makes the routing `O(E log V)`
instead of `O(V²)`.

**Dijkstra** (`main.c`) — seeds the frontier with the start's neighbours, then
settles vertices in increasing distance order. The start is marked settled
before any relaxation, which is what keeps the predecessor chain acyclic.
Stale heap entries left behind by an improved distance are skipped rather than
undone, since a settled vertex fails the `vis` test.

**Route cost** — every edge weight is the great-circle distance between its two
endpoints, in metres, so a route's total cost is its length in metres.

## Project layout

```
main.c          map loading, haversine, nearest-node lookup, Dijkstra
Graph.c/.h      weighted graph: adjacency lists + node coordinates
HashTable.c/.h  OSM id -> internal vertex index
Heap.c/.h       binary min-heap priority queue
List.c/.h       doubly-linked list used for adjacency lists
cJSON.c/.h      third-party JSON parser (vendored)
index.html      Leaflet map, click handling, route + distance display
server.js        Express server exposing the routing engine over HTTP
map.json        OSM extract of Bucharest, ~49 MB (see below)
```

## Map data

`map.json` is an [Overpass API](https://overpass-api.de/) export covering
Bucharest and its surroundings:

| | |
|---|---|
| Nodes | 251,164 |
| Ways | 54,969 |
| Bounding box | lat 44.2328 – 44.5799, lon 25.7539 – 26.5198 |
| Generator | Overpass API 0.7.62.11 |

The file is committed, so a fresh clone already has it and no download step is
needed. It is around 49 MB on disk, though JSON compresses well — the entire
`.git` directory comes to under 8 MB. To regenerate it from scratch, run a query
along these lines and save the result as `map.json`:

```
[out:json];
(
  way["highway"](44.2328,25.7539,44.5799,26.5198);
);
(._;>;);
out;
```

Adjust the bounding box to cover a different area; a larger one will produce a
larger graph and slower startup.

## Known limitations

- **Edge weights are truncated to whole metres.** `main.c` stores each edge cost
  as `int`, so every edge loses on average 0.5 m and the engine's own route cost
  reads about 2% short (114 m on a 5 km route). Edges shorter than a metre round
  down to a cost of `0` and become free to traverse. The distance shown in the
  web UI is computed independently from the polyline, at full precision, so the
  two figures do not quite agree.
- **Routes are returned destination-first.** The coordinate array runs from the
  destination back to the start. It draws identically on a map, but reverse it
  before treating it as a start-to-end itinerary.
- **The engine fails silently.** A wrong argument count, a missing map file or
  unparseable JSON all exit `1` with no message on stdout or stderr, which makes
  setup problems hard to diagnose from the browser.
- **Everything is loaded into memory on every request.** `server.js` spawns a new
  process per route, and each one re-parses the full 49 MB map, so the first
  response takes a couple of seconds.
