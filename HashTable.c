#include <stdlib.h>
#include <stdio.h>
#include "HashTable.h"

// Map an OSM id onto a bucket index
int hash(long long osm_id, int size) {
	return osm_id % size;
}

// Allocate a table with `size` empty buckets
HashTable initHashTable(int size) {
	HashTable hashTable = malloc(sizeof(struct hashTable));
	hashTable->size = size;
	hashTable->buckets = calloc(size, sizeof(struct hashNode*));
	return hashTable;
}

// Prepend a new entry to its bucket; a repeated key shadows the old mapping
void put(HashTable table, long long osm_id, int internal_id) {
	if (table == NULL) {
		return;
	}
	unsigned int index = hash(osm_id, table->size);
	HashNode head = table->buckets[index], new;
	new = malloc(sizeof(struct hashNode));
	new->next = head;
	new->internal_id = internal_id;
	new->osm_id = osm_id;
	table->buckets[index] = new;
}

// Walk the bucket's collision chain looking for osm_id
int get(HashTable table, long long osm_id) {
	if (table == NULL) {
		return -1;
	}
	unsigned int index = hash(osm_id, table->size);
	HashNode iter = table->buckets[index];
	while (iter != NULL && iter->osm_id != osm_id) {
		iter = iter->next;
	}
	if (iter == NULL) {
		return -1;
	}
	return iter->internal_id;
}

// Free every collision chain, then the bucket array, then the table itself
HashTable freeHashTable(HashTable table) {
	if (table == NULL) {
		return NULL;
	}
	if (table->buckets != NULL) {
		for (int i = 0; i < table->size; i++) {
			// Save `next` before freeing, or the walk reads freed memory
			HashNode iter = table->buckets[i], next;
			while (iter != NULL) {
				next = iter->next;
				free(iter);
				iter = next;
			}
		}
		free(table->buckets);
	}
	free(table);
	return NULL;
}