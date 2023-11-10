/* 
 * locked-queue implementation
 *
 * Authors: Daniel Jeon, Bill Zhang, Walker Ball, Dhruv Chandra, Fall 2023
 */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <queue.h>
#include <pthread.h>
#include <lqueue.h>

typedef struct lqueue {
    queue_t * queue;
    pthread_mutex_t mutex;
} lqueue_s;

/* create an empty locked-queue */
lqueue_t* lqopen(void) {
    lqueue_s * l = (lqueue_s *) malloc(sizeof(struct lqueue));
    l->queue = qopen();
    //initializes the mutex
    pthread_mutex_init(&(l->mutex), NULL);
    return (lqueue_t *) l;
}

/* deallocate a locked-queue, frees everything in it and the mutex*/
void lqclose(lqueue_t *lqp) {
    pthread_mutex_destroy(&(((lqueue_s *)lqp)->mutex));
    qclose((((lqueue_s *)lqp)->queue));
}

/*
* put element at the end of the locked-queue
* returns 0 is successful; nonzero otherwise
*/
int32_t lqput(lqueue_t *lqp, void *elementp) {
    return qput((((lqueue_s *)lqp)->queue), elementp);
}

/* get the first element from the locked queue, removing it from the queue */
void* lqget(lqueue_t *lqp) {
   return qget(((lqueue_s *)lqp)->queue);
}

/* apply a function to every element of the locked-queue */
void lqapply(lqueue_t *lqp, void (*fn)(void* elementp)) {
    qapply(((lqueue_s *)lqp)->queue, fn);
}

/* search a locked-queue using a supplied boolean function
 * skeyp -- a key to search for
 * searchfn -- a function applied to every element of the queue
 *          -- elementp - a pointer to an element
 *          -- keyp - the key being searched for (i.e. will be 
 *             set to skey at each step of the search
 *          -- returns TRUE or FALSE as defined in bool.h
 * returns a pointer to an element, or NULL if not found
 */
void* lqsearch(lqueue_t *lqp, 
							bool (*searchfn)(void* elementp,const void* keyp),
							const void* skeyp){
    return qsearch(((lqueue_s *)lqp)->queue, searchfn, skeyp);
}


