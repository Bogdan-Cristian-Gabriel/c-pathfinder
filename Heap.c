#include "Heap.h"
#include <stdio.h>
#include <stdlib.h>

// Create an empty heap
Heap initHeap(int capacity) {
	Heap h = (Heap)malloc(sizeof(struct heap));
	h->size = 0;
	h->capacity = capacity;
	h->vector = malloc(capacity * sizeof(Type));
	return h;
}

// Min-heap comparator: a < b ⇒ negative
int compHeap(Type a, Type b) {
	return a.cost - b.cost;
}

// Sift down from index to restore the heap property
Heap siftDown(Heap h, int index) {
	int minIndex = index;

	// Check left child
	int l = index * 2 + 1;
	if (l < h->size && compHeap(h->vector[l], h->vector[minIndex]) < 0) {
		minIndex = l;
	}

	// Check right child
	int r = index * 2 + 2;
	if (r < h->size && compHeap(h->vector[r], h->vector[minIndex]) < 0) {
		minIndex = r;
	}

	// If a child is smaller, swap and continue down
	if (index != minIndex) {
		Type aux = h->vector[index];
		h->vector[index] = h->vector[minIndex];
		h->vector[minIndex] = aux;
		h = siftDown(h, minIndex);
	}
	return h;
}

// Sift up from index while parent is larger
Heap siftUp(Heap h, int index) {
	while (index > 0 &&
	       compHeap(h->vector[(index - 1) / 2], h->vector[index]) > 0) {
		// Swap with parent
		Type aux = h->vector[(index - 1) / 2];
		h->vector[(index - 1) / 2] = h->vector[index];
		h->vector[index] = aux;
		index = (index - 1) / 2;
	}
	return h;
}

// Insert an element; double capacity if full
Heap insertHeap(Heap h, Type element) {
	if (h->size == h->capacity) {
		h->capacity *= 2;
		h->vector = realloc(h->vector, h->capacity * sizeof(Type));
	}

	// Place at end and sift up
	h->vector[h->size] = element;
	h->size++;
	h = siftUp(h, h->size - 1);
	return h;
}

// Remove and return min (root). Exits if heap is empty.
Type extractMin(Heap h) {
	if (h && h->size > 0) {
		Type min = h->vector[0];

		// Move last element to root and sift down
		h->vector[0] = h->vector[h->size - 1];
		h->size--;
		h = siftDown(h, 0);
		return min;
	}

	fprintf(stderr, "extractMin: heap is empty\n");
	exit(1);
}

// Free vector and struct
Heap freeHeap(Heap h) {
	if (h != NULL) {
		free(h->vector);
	}
	free(h);
	return NULL;
}