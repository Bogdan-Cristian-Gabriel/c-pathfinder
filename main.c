#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "cJSON.h"
#include "HashTable.h"
#include "Graph.h"
#include "Heap.h"

// Cost used to mark a vertex that has not been reached yet
#define INF __INT_MAX__

// Shortest route from start to dest, over the graph's edge costs.
// Returns a newly allocated array of vertex indices ordered dest -> start
// and terminated by -1; the caller owns it.
int *dijkstra(Graph g, int start, int dest) {\
	int *par = calloc(g->V, sizeof(int)); // predecessor of each vertex on the route
	int *dis = calloc(g->V, sizeof(int)); // best known distance from start
	int *vis = calloc(g->V, sizeof(int)); // 1 once a vertex is settled
	Heap h = initHeap(g->V);
	// Settle the start up front so it can never be relaxed into its own
	// successor; that would give the parent chain a cycle.
	vis[start] = 1;
	par[start] = -1;
	// Seed the frontier with start's neighbours; every other vertex begins
	// unreachable.
	for (int i = 0; i < g->V; i++) {
		List iter = g->adjLists[start];
		while (iter != NULL) {
			if (iter->data.v == i) {
				dis[i] = iter->data.cost;
				par[i] = start;
				Pair p = {i, iter->data.cost};
				h = insertHeap(h, p);
				break;
			}
			iter = iter->next;
		}
		if (iter == NULL) {
			dis[i] = INF;
			par[i] = -1; 
		}
	}
	// Settle vertices in increasing distance order, relaxing their edges
	while (h->size != 0) {
		Pair u = extractMin(h);
		vis[u.v] = 1;
		List iter = g->adjLists[u.v];
		while (iter != NULL) {
			// A settled vertex fails the vis test, so the stale heap entries
			// left behind by an improved distance are ignored rather than undone.
			if (vis[iter->data.v] == 0 && dis[iter->data.v] > dis[u.v] + iter->data.cost) {
				dis[iter->data.v] = dis[u.v] + iter->data.cost;
				par[iter->data.v] = u.v;
				Pair p = {iter->data.v, dis[iter->data.v]};
				h = insertHeap(h, p);
			}
			iter = iter->next;
		}
	}
	// Walk the predecessor chain backwards from the destination. At most V
	// vertices, plus the -1 terminator.
	int *way = calloc(g->V + 1, sizeof(int));
	int dim = -1;
	int node = dest;
	while (node != -1) {
		way[++dim] = node;
		node = par[node];
	}
	way[++dim] = -1;
	free(par);
	free(dis);
	free(vis);
	h = freeHeap(h);
	return way;
}

// Great-circle distance between two lat/lon pairs, in metres. This is the
// weight of every edge, so route costs come out in metres too.
double haversine(double lat1, double lon1, double lat2, double lon2) {
	double r = 6371000.0; // Earth radius in metres
	// Convert the inputs from degrees to radians
	lat1 = M_PI * lat1 / 180.0;
	lon1 = M_PI * lon1 / 180.0;
	lat2 = M_PI * lat2 / 180.0;
	lon2 = M_PI * lon2 / 180.0;

	double dlat = lat2 - lat1;
	double dlon = lon2 - lon1;

	double havLat = pow(sin(dlat / 2.0), 2.0);
	double havLon = pow(sin(dlon / 2.0), 2.0);

	// Central angle between the two points
	double a = havLat + cos(lat1) * cos(lat2) * havLon;
	double c = 2.0 * atan2(sqrt(a), sqrt(1.0 - a));

	return r * c;
}

int getClosestNode(Graph g, double targetLat, double targetLon) {
	double minDist = 99999999.0;
	int closestId = -1;
	for (int i = 0; i < g->V; i++) {
		int dist = (int)haversine(targetLat, targetLon, g->nodes[i].lat, g->nodes[i].lon);
		if (dist < minDist) {
			minDist = dist;
			closestId = i;
		}
	}
	return closestId;
}

int main(int argc, char *argv[]) {
	if (argc != 5) return 1;

	// Read the whole Overpass export into memory
	FILE *map = fopen("map.json", "rb");

	if (!map) {
		return 1;
	}

	fseek(map, 0, SEEK_END);
	long mapSize = ftell(map);
	rewind(map);
	
	char *buffer = malloc(sizeof(char) * (mapSize + 1));
	fread(buffer, mapSize, 1, map);
	buffer[mapSize] = '\0';

	fclose(map);

	cJSON *json = cJSON_Parse(buffer);
	if (!json) {
		return 1;
	}
	free(buffer);

	cJSON *elements = cJSON_GetObjectItemCaseSensitive(json, "elements");
	int V = 0;
	cJSON *element = NULL;

	// First pass: count the nodes so the graph and hash table can be
	// allocated at their final size
	cJSON_ArrayForEach(element, elements) {
		cJSON *type = cJSON_GetObjectItemCaseSensitive(element, "type");
		if (cJSON_IsString(type) && strcmp(type->valuestring, "node") == 0) {
			V++;
		}
	}

	Graph g = initGraph(V);
	HashTable ht = initHashTable(V * 2);

	double start_lat = atof(argv[1]);
	double start_lon = atof(argv[2]);
	double dest_lat = atof(argv[3]);
	double dest_lon = atof(argv[4]);

	// Second pass: give every node an internal index and record its
	// coordinates, keyed by the OSM id that the ways refer to
	int internal_id = 0;
	cJSON_ArrayForEach(element, elements) {
		cJSON *type = cJSON_GetObjectItemCaseSensitive(element, "type");
		if (cJSON_IsString(type) && strcmp(type->valuestring, "node") == 0) {
			long long osm_id = (long long)cJSON_GetObjectItemCaseSensitive(element, "id")->valuedouble;
			double lat = cJSON_GetObjectItemCaseSensitive(element, "lat")->valuedouble;
			double lon = cJSON_GetObjectItemCaseSensitive(element, "lon")->valuedouble;
			put(ht, osm_id, internal_id);
			g->nodes[internal_id].lat = lat;
			g->nodes[internal_id++].lon = lon;
		}
	}

	// Third pass: turn every consecutive pair of nodes in a way into an
	// undirected edge weighted by its length in metres
	cJSON_ArrayForEach(element, elements) {
		cJSON *type = cJSON_GetObjectItemCaseSensitive(element, "type");
		if (cJSON_IsString(type) && strcmp(type->valuestring, "way") == 0) {
			cJSON *nodes_array = cJSON_GetObjectItemCaseSensitive(element, "nodes");
			int nr_nodes = cJSON_GetArraySize(nodes_array);
			for (int i = 0; i < nr_nodes - 1; i++) {
				long long curr_osm_id = (long long)cJSON_GetArrayItem(nodes_array, i)->valuedouble;
				long long next_osm_id = (long long)cJSON_GetArrayItem(nodes_array, i + 1)->valuedouble;
				int u = get(ht, curr_osm_id);
				int v = get(ht, next_osm_id);

				// A way may reference a node that was not among the
				// extracted nodes; get() reports those with -1.
				if (u != -1 && v != -1) {
					int dist = (int)haversine(g->nodes[u].lat, g->nodes[u].lon, g->nodes[v].lat, g->nodes[v].lon);
					g = insertEdge(g, u, v, dist);
					g = insertEdge(g, v, u, dist);
				}
			}
		}
	}

	cJSON_Delete(json);

	int start = getClosestNode(g, start_lat, start_lon);
	int dest = getClosestNode(g, dest_lat, dest_lon);

	// The route is a list of internal vertex indices, dest first and start
	// last, terminated by -1.
	int *route = dijkstra(g, start, dest);

	printf("[\n");
	int i = 0;
	while (route[i] != -1) {
		printf("  [%f, %f]", g->nodes[route[i]].lon, g->nodes[route[i]].lat);
		if (route[i + 1] != -1) {
			printf(",\n");
		} else {
			printf("\n");
		}
		i++;
	}
	printf("]\n");

	free(route);
	ht = freeHashTable(ht);
	g = freeGraph(g);
	return 0;
}