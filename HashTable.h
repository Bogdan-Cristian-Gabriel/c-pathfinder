#include "List.h"

typedef struct hashNode {
	long long osm_id;
	int internal_id;
	struct hashNode *next;
} *HashNode;

typedef struct hashTable {
	HashNode *buckets;
	int size;
} *HashTable;
