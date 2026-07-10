#include <stdio.h>
#include <stdlib.h>
#include "List.h"

List addFirst(List l, Pair data) {
	List tmp, new = (List)malloc(sizeof(struct list));
	new->data = data;
	new->prev = NULL;
	if (l == NULL) {
		new->next = NULL;
		return new;
	}
	new->next = l;
	l->prev = new;
	return new;
}