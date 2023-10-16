/* 
 * Daniel Jeon, Bill Zheng, Dhruv Chandra, Walker Ball
 * Module 3: Queue Module
 * 
 * Implements the queue.h
 */
#include <stdint.h>
#include <stdbool.h>
#include "queue.h"
#include <stdlib.h>
#include <stdio.h>

/* must use nodes unless the test has specific nodes that have the variable "next" */
typedef struct node{
    void* data;
    struct node* next;
} node_t;

/* the queue representation is hidden from users of the module */
typedef struct queue{
    node_t* front;
    node_t* back;
} queue_s;

/* create an empty queue */
queue_t* qopen(void) {
    queue_s* queue = malloc(sizeof(struct queue));
    queue->front = NULL;
    queue->back = NULL;//must change the back node to something else once we start putting new nodes
		queue_t* realQueue = (queue_t*) queue;
    return realQueue;
}

/* deallocate a queue, frees everything in it */
void qclose(queue_t *qp){
    void *monke; //data
    node_t *temp;  //temporary node
    while (((queue_s*)qp)->front != NULL) {
		temp = ((queue_s*)qp)->front;
        monke = temp->data;
        ((queue_s*) qp)->front = temp->next;
        free(monke); 
        free(temp); 
        monke = NULL;
        temp = NULL;
    }
}   

/* put element at the end of the queue
 * returns 0 is successful; nonzero otherwise 
 */
int32_t qput(queue_t *qp, void *elementp){
    node_t *new = malloc(sizeof(node_t));
    if (new) {
			new->data = elementp;
			new->next = NULL;
			if (((queue_s*)qp)->front == NULL){
				((queue_s*)qp)->front = new;
				((queue_s*)qp)->back = new;
			}
			else{
				((queue_s*)qp)->back->next = new;
				((queue_s*)qp)->back = new;
			}
			return 0;
    }
    //if unsuccessful
    else {
        return 1;
    }
} 

/* get the first first element from queue, removing it from the queue */
void* qget(queue_t *qp){
    node_t *monke;
    void *data; //?
    if (((queue_s*)qp)->front == NULL) {
        printf("Error: Nothing in the queue.\n");
        return NULL;
    }
    else {
			monke = ((queue_s*)qp)->front;
			data = monke->data; //review this portion
			((queue_s*)qp)->front = monke->next;
			//	free(qp->front->data);
			//	free(((queue_s*)qp)->front);
			free(monke);
			return data;
    }
}

/* apply a function to every element of the queue */
void qapply(queue_t *qp, void (*fn)(void* elementp)){
    node_t *temp;
    for(temp = ((queue_s*)qp)->front; temp != NULL; temp = temp->next){
        fn(temp->data);
    }
}

/* search a queue using a supplied boolean function
 * skeyp -- a key to search for
 * searchfn -- a function applied to every element of the queue
 *          -- elementp - a pointer to an element
 *          -- keyp - the key being searched for (i.e. will be 
 *             set to skey at each step of the search
 *          -- returns TRUE or FALSE as defined in bool.h
 * returns a pointer to an element, or NULL if not found
 */
void* qsearch(queue_t *qp, bool (*searchfn)(void* elementp,const void* keyp), const void* skeyp){
    node_t *temp;
    for(temp = ((queue_s*)qp)->front; temp != NULL; temp = temp->next){
        if (searchfn(temp->data, skeyp)) {
            return temp->data;
        }
    }
    return NULL;
}

/* search a queue using a supplied boolean function (as in qsearch),
 * removes the element from the queue and returns a pointer to it or
 * NULL if not found
 */
void* qremove(queue_t *qp, bool (*searchfn)(void* elementp,const void* keyp), const void* skeyp) {
    node_t *temp;
    node_t *monke;
    void *data;
		temp = ((queue_s*)qp)->front;
		if (temp == NULL)
			{
				return NULL;
			}
		
		if (searchfn(temp->data, skeyp)){
			data = temp->data;
			((queue_s*)qp)->front = temp->next;
			if(((queue_s*)qp)->front == NULL){
				((queue_s*)qp)->back = NULL;
			}
			free(temp);
			return data;
		}
		else{
			for(temp = ((queue_s*)qp)->front; temp != NULL; temp = temp->next){
					if (temp->next != NULL){
						if (searchfn(temp->next->data, skeyp)) {                                                    
							data = temp->next->data;
							monke = temp->next;
							temp->next = temp->next->next;
							if (temp->next == NULL){
									((queue_s*)qp)->back = temp;
							}
						  free(monke);
							return data;
					    }
                    }
            }
    }
    return NULL;
}

/* concatenates elements of q2 into q1
 * q2 is deallocated, closed, and unusable upon completion 
 */
void qconcat(queue_t *q1p, queue_t *q2p){
	if (((queue_s*)q1p)->front == NULL){
		((queue_s*)q1p)->front = ((queue_s*)q2p)->front;
		((queue_s*)q1p)->back = ((queue_s*)q2p)->back;
	}
	else if (((queue_s*)q1p)->front != NULL && ((queue_s*)q2p)->front != NULL){
		((queue_s*)q1p)->back->next = ((queue_s*)q2p)->front;
		((queue_s*)q1p)->back = ((queue_s*)q2p)->back;
	}
}
