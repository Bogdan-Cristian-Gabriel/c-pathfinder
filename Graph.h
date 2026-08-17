// Weighted graph with adjacency lists and geographic coordinates
#ifndef GRAPH_H
#define GRAPH_H

#include "List.h"

// Node coordinates (latitude, longitude)
typedef struct nodeData {
	double lat, lon;
} NodeData;

// Graph: V vertices, adjacency lists, per-node coordinates
typedef struct graph {
	int V;            // number of vertices
	List *adjLists;   // array of V adjacency lists
	NodeData *nodes;  // array of V coordinates
} *Graph;

// Initialize a graph with V vertices, no edges
Graph initGraph(int V);
// Add directed edge u → v with a cost
Graph insertEdge(Graph g, int u, int v, int cost);
// Return cost of edge u → v, or INT_MAX if it doesn't exist
int getCost(Graph g, int u, int v);
// Free all memory used by the graph
Graph freeGraph(Graph g);

#endif