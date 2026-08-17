#include <stdio.h>
#include <stdlib.h>
#include "Graph.h"
#include "Heap.h"

#define INF __INT_MAX__

int *dijkstra(Graph g, int start, int dest) {\
	int *par = calloc(g->V, sizeof(int));
	int *dis = calloc(g->V, sizeof(int));
	int *vis = calloc(g->V, sizeof(int));
	Heap h = initHeap(g->V);
	vis[start] = 1;
	par[start] = -1;
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
	while (h->size != 0) {
		Pair u = extractMin(h);
		vis[u.v] = 1;
		List iter = g->adjLists[u.v];
		while (iter != NULL) {
			if (vis[iter->data.v] == 0 && dis[iter->data.v] > dis[u.v] + iter->data.cost) {
				dis[iter->data.v] = dis[u.v] + iter->data.cost;
				par[iter->data.v] = u.v;
				Pair p = {iter->data.v, dis[iter->data.v]};
				h = insertHeap(h, p);
			}
			iter = iter->next;
		}
	}
	int *way = calloc(g->V, sizeof(int));
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

// Semnătura funcției tale
int *dijkstra(Graph g, int start, int dest);

int main() {
    int V = 6; // O hartă cu 6 intersecții (de la 0 la 5)
    Graph g = initGraph(V);

    // Definim străzile: insertEdge(graf, plecare, sosire, timp_parcurgere)
    // Să presupunem că vrem să ajungem de la 0 la 5.
    insertEdge(g, 0, 1, 4);
    insertEdge(g, 0, 2, 2); // Scurtătura de la început
    insertEdge(g, 1, 3, 5);
    insertEdge(g, 2, 1, 1); // Stradă de legătură
    insertEdge(g, 2, 3, 8); // Stradă lungă, ar trebui evitată
    insertEdge(g, 2, 4, 10);
    insertEdge(g, 3, 5, 3);
    insertEdge(g, 4, 5, 2);

    int start = 0;
    int dest = 5;
    
    printf("--- Navigare waze-clone-c ---\n");
    printf("Calculam ruta de la %d la %d...\n", start, dest);
    
    int *ruta = dijkstra(g, start, dest);

    // Vectorul 'way' este construit de la destinație spre start (ex: 5, 3, 1, 2, 0)
    // Trebuie să calculăm câte elemente are pentru a-l printa în ordinea corectă
    int lungime = 0;
    while(ruta[lungime] != -1) {
        lungime++;
    }

    printf("Traseul optim este: ");
    for (int i = lungime - 1; i >= 0; i--) {
        printf("%d", ruta[i]);
        if (i > 0) printf(" -> ");
    }
    printf("\n");

    // Curățăm memoria alocată în Dijkstra și pentru Graf
    free(ruta);
    freeGraph(g);

    return 0;
}