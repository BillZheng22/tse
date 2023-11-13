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
#include <queue.h>
#include <hash.h>
#include <pageio.h>
#include <ctype.h>
#include <index.h>
#include <indexio.h>
#include <pthread.h>

static int total = 0;

static index_t *indexBuild(char *pageDirectory);
static void indexPage(index_t *index, webpage_t *page, int id);

void printCounts(void *cp) {
    counter_t *c = (counter_t *)cp;
    if (c == NULL)
    {
    printf("doclist is empty.");
    return;
    }
    printf("docID=%d, count=%d\n", c->docid, c->count);
    return;
}

void accessQueues(void *wmp){
    wordmap_t *wm = (wordmap_t *)wmp;
    if (wm == NULL)
    {
    printf("wordmap is empty.");
    return;
    }
    qapply(wm->doclist, printCounts);
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

int main(int argc, char *argv[]){
    char pagedir[50]; // pagedir
    char indexnm[50]; // filename
    int threadnum;    //number of threads 

    // checks if there are 3 arguments
    if (argc != 4)
    {
    printf("usage: indexer <pagedir> <indexnm> <threadnum>\n");
    exit(EXIT_FAILURE);
    }
    //string to int
    threadnum = atoi(argv[3]);
    //set the variables
    strcpy(pagedir, argv[1]);
    strcpy(indexnm, argv[2]);

    index_t *index = indexBuild(pagedir);

    indexsave(index, pagedir, indexnm);

    return 0;
}

static index_t *indexBuild(char *pageDirectory){
    index_t *index = index_new(500); // between 300 and 900 slots
    int id = 1;

    webpage_t *page;

    while ((page = pageload(id, pageDirectory)) != NULL) {// null check
        //page = pageload(id, pageDirectory); // Loads a webpage from the document file 'pageDirectory/id'
        if (page != NULL){ // if null do nothing
            printf("FILE ID RIGHT NOW OVER HERE OMG OMG --------------------> %d\n", id);
            indexPage(index, page, id);
            id++;
        }
        webpage_delete(page);
        }
    webpage_delete(page);
    return index;
}

void indexPage(index_t *index, webpage_t *page, int id) {
    int pos = 0;
    // int c = 0;
    char *word = NULL;
    int *idp = &id;
    queue_t *queue;
    counter_t *elemc;
    wordmap_t *wmap;
    wordmap_t *wordmap;
    counter_t *counter;

    while ((pos = webpage_getNextWord(page, pos, &word)) > 0)
    {
    if (normalizeWord(word) != NULL){
        total++;
        fflush(stdout);
        printf("%s\n", word);
        fflush(stdout);

        if ((wmap = (wordmap_t *)(hsearch((hashtable_t *)index, wordSearch, word, strlen(word)))) != NULL){
        if ((elemc = (counter_t *)(qsearch(wmap->doclist, queueSearch, idp))) != NULL){
            elemc->count += 1;
        }
        else {
            // put new counter into the doclist queue
            // needs to have the new id and a count of 1.
            counter = (counter_t *)malloc(sizeof(counter_t) + 1);
            counter->docid = id;
            counter->count = 1;
            qput(wmap->doclist, counter);
        }
        // printf("Succeeded hsearch and qsearch.\n");
        }
        else { // the word is not in the index yet
            printf("NOT FOUND in index yet: %s\n", word);
            // open queue
            queue = qopen();
            // create counter with id and count
            counter = (counter_t *)malloc(sizeof(counter_t) + 1);
            counter->docid = id;
            counter->count = 1;
            // add counter to the doclist
            qput(queue, counter);
            // create new wordMap
            wordmap = (wordmap_t *)malloc(sizeof(wordmap_t) + 1);
            // add queue to the wordMap
            wordmap->word = malloc(strlen(word) + 1);
            strcpy(wordmap->word, word);
            wordmap->doclist = queue;
            printf("wordmap key: %s\n", wordmap->word);
            // hput wordMap
            hput(index, wordmap, word, strlen(word));
            // printf("address of wordmap: %p\n", wordmap);
        }
    }
    free(word);
    }
    printf("TOTAL WORDS: %d\n", total);
}