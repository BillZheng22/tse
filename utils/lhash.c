/* 
 * Daniel Jeon, Bill Zheng, Dhruv Chandra, Walker Ball, Fall 2023
 * lhash.c -- implements a locked-hash table as an indexed set of queues.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <queue.h>
#include <hash.h>
#include <lhash.h> /* lhash.c implement this interface */

#define get16bits(d) (*((const uint16_t *) (d)))

typedef struct lhash{
    hashtable_t * hash;
    pthread_mutex_t mutex;
} lhash_s;

/* hopen -- opens a hash table with initial size hsize */
lhash_t *lhopen(uint32_t hsize) {
  lhash_s * ht = (lhash_s *) malloc(sizeof(lhash_s));

  //initialized the hashtable and the mutex
  pthread_mutex_init(&(ht->mutex), NULL);
  ht->hash = hopen(hsize);

  //cast to return correct type
  lhash_t * realhash = (lhash_t *) ht;
  return realhash;
}

/* hclose -- closes a hash table */
void lhclose(lhash_t *htp){
  hclose(((lhash_s *)htp)->hash);
  pthread_mutex_destroy(&(((lhash_s *)htp)->mutex));
  free(htp);
}

/* hput -- puts an entry into a hash table under designated key 
 * returns 0 for success; non-zero otherwise
 */
int32_t lhput(lhash_t *htp, void *ep, const char *key, int keylen){
  return hput(((lhash_s *)htp)->hash, ep, key, keylen);
}

/* happly -- applies a function to every entry in hash table */
void lhapply(lhash_t *htp, void (*fn)(void* ep)){
  happly(((lhash_s *)htp)->hash, fn);
}

/* hsearch -- searchs for an entry under a designated key using a
 * designated search fn -- returns a pointer to the entry or NULL if
 * not found
 */
void *lhsearch(lhash_t *htp, 
	      bool (*searchfn)(void* elementp, const void* searchkeyp), 
	      const char *key, 
	      int32_t keylen){
  return hsearch(((lhash_s *)htp)->hash, searchfn, key, keylen);
}

/* hremove -- removes and returns an entry under a designated key
 * using a designated search fn -- returns a pointer to the entry or
 * NULL if not found
 */
void *lhremove(lhash_t *htp, 
	      bool (*searchfn)(void* elementp, const void* searchkeyp), 
	      const char *key, 
	      int32_t keylen) {
    return hremove(((lhash_s *)htp)->hash, searchfn, key, keylen);
}

/* locks the mutex in the lhash structure */
void locklhash(lhash_t *ht){
  pthread_mutex_lock(&(((lhash_s *)ht)->mutex));
}

/* locks the mutex in the lhash structure */
void unlocklhash(lhash_t *ht){
  pthread_mutex_unlock(&(((lhash_s *)ht)->mutex));
}