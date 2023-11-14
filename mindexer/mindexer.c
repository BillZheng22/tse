/*
* mindexer.c --- a multi-threaded implementation of the indexer
*
* Author: Daniel Jeon, Bill Zheng, Walker Ball, Dhruv Chandra
* Created: Fall 2023
* Version: 1.0
*
* Description: a program that implements the pageio.h module
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <webpage.h>
#include <lqueue.h>
#include <lhash.h>
#include <pageio.h>
#include <ctype.h>
#include <mindex.h>
#include <mindexio.h>
#include <pthread.h>

/* Global Variables */
static int total = 0;
static int id = 1;
static char pagedir[50]; // pagedir
static pthread_mutex_t mutex; //mutex to control the next file to be indexed

static mindex_t *mindexBuild(int threadnum);
static void mindexPage(mindex_t *index, webpage_t *page, int id);

void printCounts(void *cp) {
    counter_t *c = (counter_t *)cp;
    if (c == NULL){
        //printf("doclist is empty.\n");
        return;
    }
    printf("docID=%d, count=%d\n", c->docid, c->count);
    return;
}

void accessQueues(void *wmp){
    wordmap_t *wm = (wordmap_t *)wmp;
    if (wm == NULL) {
        //printf("wordmap is empty.\n");
        return;
    }
    lqapply(wm->doclist, printCounts);
    return;
}

bool wordSearch(void *elementp, const void *searchkeyp){
    wordmap_t *wordmap = (wordmap_t *)elementp;
    return strcmp(wordmap->word, (const char *)searchkeyp) == 0;
}

bool queueSearch(void *elementp, const void *searchkeyp){
    counter_t *counter = (counter_t *)elementp;
    int searchint = *(int *)searchkeyp;
    return (counter->docid == searchint);
}

/* makes the word lowercase */
char *normalizeWord(char *word){
    int cCounter;
    char c;
    // Check is word is greater than 3 characters
    int wordLen = strlen(word);
    if (wordLen < 3){
        return NULL;
    }
    // Check is word contains alphanumeric characters
    for (cCounter = 0; cCounter < wordLen; cCounter++){
        c = word[cCounter];
        if ((isalpha(c)) == 0){
            return NULL;
        }
    }
    // Check if word is uppercase
    for (cCounter = 0; cCounter < wordLen; cCounter++) {
        c = tolower(word[cCounter]);
        word[cCounter] = c;
    }
    return word;
}

//concurrent threads sharing the index to build the pages
void *coindexBuild(void *argp) {
    //locks the mutex so the current thread can only scan the pages
    pthread_mutex_lock(&mutex);
    //cast the argp to an index
    mindex_t * index = (mindex_t *) argp;
    webpage_t *page;
    while ((page = pageload(id, pagedir)) != NULL) {// null check
        //page = pageload(id, pageDirectory); // Loads a webpage from the document file 'pageDirectory/id'
        if (page != NULL){ // if null do nothing
            mindexPage(index, page, id);
            id++;
        }
        webpage_delete(page);
    }
    webpage_delete(page);

    //unlocks the mutex so that other threads scan the pages
    pthread_mutex_unlock(&mutex);
    return (void*)0;
}

static mindex_t *mindexBuild(int threadnum){
    //make a new index up here
    mindex_t *index = mindex_new(500); // between 300 and 900 slots
    //initialize the mutex
    pthread_mutex_init(&mutex, NULL);
    pthread_t threads[threadnum]; //array of threads
    int i; //incrementer
    //make the threads
    for (i = 0; i < threadnum; i++) {
        if(pthread_create(&threads[i],NULL,coindexBuild,index)!=0) {
            exit(EXIT_FAILURE);
        }
    }
    int j; //incrementer for the threads to run their functions
    for (j = 0; j < threadnum; j++) {
        if (pthread_join(threads[j], NULL) != 0) {
            exit(EXIT_FAILURE);
        }
    }
    pthread_mutex_destroy(&mutex);
    return index;
}

void mindexPage(mindex_t *index, webpage_t *page, int id) {
    int pos = 0;
    char *word = NULL;
    int *idp = &id;
    lqueue_t *queue;
    counter_t *elemc;
    wordmap_t *wmap;
    wordmap_t *wordmap;
    counter_t *counter;

    while ((pos = webpage_getNextWord(page, pos, &word)) > 0){
    if (normalizeWord(word) != NULL){
        total++;
        fflush(stdout);
        printf("%s\n", word);
        fflush(stdout);

        if ((wmap = (wordmap_t *)(lhsearch((lhash_t *)index, wordSearch, word, strlen(word)))) != NULL){
        if ((elemc = (counter_t *)(lqsearch(wmap->doclist, queueSearch, idp))) != NULL){
            elemc->count += 1;
        }
        else {
            // put new counter into the doclist queue
            // needs to have the new id and a count of 1.
            counter = (counter_t *)malloc(sizeof(counter_t) + 1);
            counter->docid = id;
            counter->count = 1;
            lqput(wmap->doclist, counter);
        }
        // printf("Succeeded hsearch and qsearch.\n");
        }
        else { // the word is not in the index yet
            printf("NOT FOUND in index yet: %s\n", word);
            // open queue
            queue = lqopen();
            // create counter with id and count
            counter = (counter_t *)malloc(sizeof(counter_t) + 1);
            counter->docid = id;
            counter->count = 1;
            // add counter to the doclist
            lqput(queue, counter);
            // create new wordMap
            wordmap = (wordmap_t *)malloc(sizeof(wordmap_t) + 1);
            // add queue to the wordMap
            wordmap->word = malloc(strlen(word) + 1);
            strcpy(wordmap->word, word);
            wordmap->doclist = queue;
            printf("wordmap key: %s\n", wordmap->word);
            // hput wordMap
            lhput(index, wordmap, word, strlen(word));
            // printf("address of wordmap: %p\n", wordmap);
        }
    }
    free(word);
    }
    printf("TOTAL WORDS: %d\n", total);
}

int main(int argc, char *argv[]){
    char indexnm[50]; // filename
    int threadnum;    //number of threads 
    //string to int
    threadnum = atoi(argv[3]);

    // checks if there are 3 arguments
    if (argc != 4) {
        printf("usage: mindexer <pagedir> <indexnm> <threadnum>\n");
        exit(EXIT_SUCCESS);
    }

    //set the variables
    strcpy(pagedir, argv[1]);
    strcpy(indexnm, argv[2]);

    //builds the index
    mindex_t *index = mindexBuild(threadnum);
    //saves the index
    mindexsave(index, pagedir, indexnm);

    exit(EXIT_SUCCESS);
}