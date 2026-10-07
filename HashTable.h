// Hash table mapping OSM node ids to internal graph indices

#include "List.h"

// One entry in a bucket's collision chain
typedef struct hashNode {
	long long osm_id;      // OpenStreetMap id, used as the key
	int internal_id;       // index of this node in Graph::nodes
	struct hashNode *next; // next entry in the same bucket
} *HashNode;

// Separate-chaining hash table: an array of bucket heads
typedef struct hashTable {
	HashNode *buckets; // array of `size` bucket heads
	int size;          // number of buckets
} *HashTable;

// Map osm_id onto a bucket index in [0, size)
int hash(long long osm_id, int size);
// Allocate a table with the given number of empty buckets
HashTable initHashTable(int size);
// Insert osm_id; a repeated key shadows the previous mapping
void put(HashTable table, long long osm_id, int internal_id);
// Return the internal id mapped to osm_id, or -1 if absent
int get(HashTable table, long long osm_id);
// Free every chain, the bucket array and the table itself; returns NULL
HashTable freeHashTable(HashTable table);