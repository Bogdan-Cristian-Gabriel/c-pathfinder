#ifndef GRAPH_H
#define GRAPH_H

#include "List.h"

typedef struct nodeData {
	double lat, lon;
} NodeData;

typedef struct graph {
	int V;
	List *adjLists;
	NodeData *nodes;
} *Graph;

Graph initGraph(int V);
Graph insertEdge(Graph g, int u, int v, int cost);
int getCost(Graph g, int u, int v);
Graph freeGraph(Graph g);

#endif