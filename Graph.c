#include <stdio.h>
#include <stdlib.h>
#include "Graph.h"

Graph initGraph(int V) {
	Graph g;
	g = (Graph)malloc(sizeof(struct graph));
	g->V = V;
	g->adjLists = (List*)malloc(V * sizeof(List));
	for (int i = 0; i < V; i++) {
		g->adjLists[i] = NULL;
	}
	g->nodes = (NodeData*)malloc(V * sizeof(NodeData));
	return g;
}

Graph insertEdge(Graph g, int u, int v, int cost) {
	Pair p;
	p.v = v;
	p.cost = cost;
	g->adjLists[u] = addFirst(g->adjLists[u], p);
	return g;
}

int getCost(Graph g, int u, int v) {
	List tmp = g->adjLists[u];
	while (tmp != NULL) {
		if (tmp->data.v == v) {
			return tmp->data.cost;
		}
		tmp = tmp->next;
	}
	return __INT_MAX__;
}

Graph freeGraph(Graph g) {
	if (g == NULL) {
		return NULL; 
	}
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
	if (g->nodes != NULL) {
		free(g->nodes);
	}
	free(g);
	return NULL;
}