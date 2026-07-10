#ifndef LIST_H
#define LIST_H

typedef struct pair {
	int v, cost;
} Pair;

typedef struct list {
	Pair data;
	struct list *next, *prev;
} *List;

List addFirst(List l, Pair data);

#endif