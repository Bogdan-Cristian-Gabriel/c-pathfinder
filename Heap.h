// Binary min-heap with dynamic array — priority queue
#ifndef HEAP_H
#define HEAP_H

#include "List.h"

// Element type stored in the heap (generic int)
typedef Pair Type;

// Binary heap: dynamic array + size + capacity
typedef struct heap {
	Type *vector;   // elements
	int size;       // number of elements in heap
	int capacity;   // current vector capacity
} *Heap;

// Create an empty heap with the given initial capacity
Heap initHeap(int capacity);
// Comparator: returns a - b (negative if a < b)
int compHeap(Type a, Type b);
// Sift down from index to restore heap property
Heap siftDown(Heap h, int index);
// Sift up from index to restore heap property
Heap siftUp(Heap h, int index);
// Insert an element (doubles capacity if full)
Heap insertHeap(Heap h, Type element);
// Remove and return min (root); exits program if heap is empty
Type extractMin(Heap h);
// Free all memory
Heap freeHeap(Heap h);

#endif /* HEAP_H */
