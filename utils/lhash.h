#pragma once
/*
 * lhash.h -- A generic hash table implementation, allowing arbitrary
 * key structures.
 *
 */
#include <stdint.h>
#include <stdbool.h>
#include <queue.h>
#include <hash.h>

typedef void lhash_t;	/* representation of a hashtable hidden */

/* hopen -- opens a hash table with initial size hsize */
lhash_t *lhopen(uint32_t hsize);

/* hclose -- closes a hash table */
void lhclose(lhash_t *htp);

/* hput -- puts an entry into a hash table under designated key 
 * returns 0 for success; non-zero otherwise
 */
int32_t lhput(lhash_t *htp, void *ep, const char *key, int keylen);

/* happly -- applies a function to every entry in hash table */
void lhapply(lhash_t *htp, void (*fn)(void* ep));

/* hsearch -- searchs for an entry under a designated key using a
 * designated search fn -- returns a pointer to the entry or NULL if
 * not found
 */
void *lhsearch(lhash_t *htp, 
	      bool (*searchfn)(void* elementp, const void* searchkeyp), 
	      const char *key, 
	      int32_t keylen);

/* hremove -- removes and returns an entry under a designated key
 * using a designated search fn -- returns a pointer to the entry or
 * NULL if not found
 */
void *lhremove(lhash_t *htp, 
	      bool (*searchfn)(void* elementp, const void* searchkeyp), 
	      const char *key, 
	      int32_t keylen);

/* locks the mutex in the lhash structure */
void locklhash(lhash_t *ht);

/* locks the mutex in the lhash structure */
void unlocklhash(lhash_t *ht);