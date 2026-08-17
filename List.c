#include <stdio.h>
#include <stdlib.h>
#include "List.h"

// Insert a new node at the front; return new list head
List addFirst(List l, Pair data) {
	List tmp, new = (List)malloc(sizeof(struct list));
	new->data = data;
	new->prev = NULL;

	// Empty list — new node is the only element
	if (l == NULL) {
		new->next = NULL;
		return new;
	}

	// Link new node in front of the existing list
	new->next = l;
	l->prev = new;
	return new;
}