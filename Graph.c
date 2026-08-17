#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "Graph.h"

// Initialize a graph with V vertices, no edges
Graph initGraph(int V) {
	Graph g;
	g = (Graph)malloc(sizeof(struct graph));
	g->V = V;

	// Allocate and initialize adjacency lists (all empty)
	g->adjLists = (List*)malloc(V * sizeof(List));
	for (int i = 0; i < V; i++) {
		g->adjLists[i] = NULL;
	}

	// Allocate coordinate array
	g->nodes = (NodeData*)malloc(V * sizeof(NodeData));
	return g;
}

// Add a directed edge u → v with the given cost
Graph insertEdge(Graph g, int u, int v, int cost) {
	Pair p;
	p.v = v;
	p.cost = cost;
	g->adjLists[u] = addFirst(g->adjLists[u], p);
	return g;
}

// Return cost of edge u → v, or INT_MAX if not found
int getCost(Graph g, int u, int v) {
	List tmp = g->adjLists[u];
	while (tmp != NULL) {
		if (tmp->data.v == v) {
			return tmp->data.cost;
		}
		tmp = tmp->next;
	}
	return INT_MAX;  // edge doesn't exist
}

// Free all memory: list nodes, arrays, graph struct
Graph freeGraph(Graph g) {
	if (g == NULL) {
		return NULL;
	}

	// Free each adjacency list's nodes
	if (g->adjLists != NULL) {
		for (int i = 0; i < g->V; i++) {
			List iter = g->adjLists[i];
			while(iter != NULL) {
				List tmp = iter;
				iter = iter->next;
				free(tmp);
			}
		}
		free(g->adjLists);
	}

	// Free coordinate array
	if (g->nodes != NULL) {
		free(g->nodes);
	}
	free(g);
	return NULL;
}