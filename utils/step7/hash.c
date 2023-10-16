/* 
 * hash.c -- implements a generic hash table as an indexed set of queues.
 *  
 */
#include <stdint.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "queue.h"
#include "hash.h"

#define get16bits(d) (*((const uint16_t *) (d)))

typedef struct hash{
    uint32_t hsize;
    queue_t **array;
} hashtable_s;

/* hopen -- opens a hash table with initial size hsize */
hashtable_t *hopen(uint32_t hsize) {
  int i; //iterator
  queue_t* queue;

  hashtable_s* newhash = (hashtable_s*) malloc(sizeof(hashtable_s));
  newhash->hsize = hsize;
  newhash->array = (queue_t **) malloc(hsize * sizeof(queue_t*));

  //from start mem address to end mem address malloc to give space for queues
  for (i = 0; i < hsize; i++) {
    queue = qopen();
    newhash->array[i] = queue; 
  }
  hashtable_t* realhash = (hashtable_t*) newhash;
  return realhash;
}

/* hclose -- closes a hash table */
void hclose(hashtable_t *htp){
  if (htp != NULL) {
    uint32_t i; //incrementer
    if (((hashtable_s*)htp)->array != NULL) {
      for (i=0;i<((hashtable_s*)htp)->hsize;i++) {
        qclose((((hashtable_s*)htp)->array[i]));
      }
      free(((hashtable_s*)htp)->array);
    }
    free(htp);
  }
  else {
    printf("Error: there's no hashtable initialized.\n");
  }
}

/* 
 * SuperFastHash() -- produces a number between 0 and the tablesize-1.
 * 
 * The following (rather complicated) code, has been taken from Paul
 * Hsieh's website under the terms of the BSD license. It's a hash
 * function used all over the place nowadays, including Google Sparse
 * Hash.
 */
static uint32_t SuperFastHash (const char *data,int len,uint32_t tablesize) {
  uint32_t hash = len, tmp;
  int rem;
  
  if (len <= 0 || data == NULL)
		return 0;
  rem = len & 3;
  len >>= 2;
  /* Main loop */
  for (;len > 0; len--) {
    hash  += get16bits (data);
    tmp    = (get16bits (data+2) << 11) ^ hash;
    hash   = (hash << 16) ^ tmp;
    data  += 2*sizeof (uint16_t);
    hash  += hash >> 11;
  }
  /* Handle end cases */
  switch (rem) {
  case 3: hash += get16bits (data);
    hash ^= hash << 16;
    hash ^= data[sizeof (uint16_t)] << 18;
    hash += hash >> 11;
    break;
  case 2: hash += get16bits (data);
    hash ^= hash << 11;
    hash += hash >> 17;
    break;
  case 1: hash += *data;
    hash ^= hash << 10;
    hash += hash >> 1;
  }
  /* Force "avalanching" of final 127 bits */
  hash ^= hash << 3;
  hash += hash >> 5;
  hash ^= hash << 4;
  hash += hash >> 17;
  hash ^= hash << 25;
  hash += hash >> 6;
  return hash % tablesize;
}

/* hput -- puts an entry into a hash table under designated key 
 * returns 0 for success; non-zero otherwise
 */
int32_t hput(hashtable_t *htp, void *ep, const char *key, int keylen){
  uint32_t index = SuperFastHash(key, keylen, ((hashtable_s*)htp)->hsize);
  if (htp != NULL) {
    if (qput((((hashtable_s*)htp)->array[index]), ep) != 0) {
      printf("Error: not successful in putting entry into hashtable.\n");
      return -1;
    }
    else {
      return 0;
    }
  }
  else {
    printf("Error: not successful in putting entry into hashtable, because htp points to NULL.\n");
    return -1;
  }
}

/* happly -- applies a function to every entry in hash table */
void happly(hashtable_t *htp, void (*fn)(void* ep)){
  int i;
  if (htp != NULL){
    for (i=0;i<((hashtable_s*)htp)->hsize;i++) { 
      qapply((hashtable_t*)(((hashtable_s*)htp)->array[i]), fn);
    }
  }
  else {
    printf("error: htp points to NULL.\n");
  }
}

/* hsearch -- searchs for an entry under a designated key using a
 * designated search fn -- returns a pointer to the entry or NULL if
 * not found
 */
void *hsearch(hashtable_t *htp, 
	      bool (*searchfn)(void* elementp, const void* searchkeyp), 
	      const char *key, 
	      int32_t keylen){
    if (htp != NULL) {
      uint32_t index = SuperFastHash(key, keylen, ((hashtable_s*)htp)->hsize);
      return qsearch((hashtable_t*)(((hashtable_s*)htp)->array[index]), searchfn, key);
    }
    else {
      printf("error: htp points to NULL.\n");
      return NULL;
    }
}

/* hremove -- removes and returns an entry under a designated key
 * using a designated search fn -- returns a pointer to the entry or
 * NULL if not found
 */
void *hremove(hashtable_t *htp, 
	      bool (*searchfn)(void* elementp, const void* searchkeyp), 
	      const char *key, 
	      int32_t keylen) {
    if (htp != NULL) {
      uint32_t index = SuperFastHash(key, keylen, ((hashtable_s*)htp)->hsize);
      return qremove((hashtable_t*)(((hashtable_s*)htp)->array[index]), searchfn, key);
    }
    else {
      printf("error: htp points to NULL.\n");
      return NULL;
    }
}

