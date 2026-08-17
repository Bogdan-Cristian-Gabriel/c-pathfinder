// Doubly-linked list for graph adjacency lists
#ifndef LIST_H
#define LIST_H

// An edge: destination vertex + cost
typedef struct pair {
	int v, cost;
} Pair;

// Node in an adjacency list
typedef struct list {
	Pair data;
	struct list *next, *prev;
} *List;

// Insert an element at the front of the list
List addFirst(List l, Pair data);

#endif