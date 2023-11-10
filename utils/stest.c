/*
    Daniel Jeon, Bill Zhang, Walker Ball, Dhruv Chandra
    Fall 2023
    Single-Threaded Test
*/
#include <stdio.h>
#include <stdlib.h>
#include <lqueue.h>
#include <stdbool.h>
#include <pthread.h>

void *tfunc1(void * argp) {

}

/* sample search function for elements in the locked queue */
bool searchfn(void * elementp) {

}

int main(void) {
    pthread_t tid1;
    //open a locked queue
    lqueue_t *lqp = lqopen();
    if(pthread_create(&tid1,NULL,tfunc1,argp1)!=0) {
        exit(EXIT_FAILURE);
    }
    
    lqclose(lqp);
}