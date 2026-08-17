#include <stdlib.h>
#include <stdio.h>
#include "HashTable.h"

int hash(long long osm_id, int size) {
	return osm_id % size;
}

HashTable initHashTable(int size) {
	HashTable hashTable = malloc(sizeof(struct hashTable));
	hashTable->size = size;
	hashTable->buckets = calloc(size, sizeof(struct hashNode*));
	return hashTable;
}

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

HashTable freeHashTable(HashTable table) {
	if (table == NULL) {
		return;
	}
	if (table->buckets != NULL) {
		
		free(table->buckets);
	}
}