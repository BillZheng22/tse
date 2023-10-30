#pragma once
/*
 * index.h
 *
 */
#include <stdint.h>
#include <stdbool.h>

typedef struct index index_t;
index_t* index_new(const int numSlots);
void index_delete(index_t* index);

//bool index_set(index_t* index, char* word, int docID, int count);
bool index_insert(index_t* index, int docID, char* word);

// void index_save(index_t* index, char* file);
// index_t* index_load(char* file);

