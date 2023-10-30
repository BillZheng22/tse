/* 
 * indexer.c --- 
 * 
 * Author: Bill Zheng
 * Created: 10-24-2023
 * Version: 1.0
 * 
 * Description: 
 * 
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

typedef struct counter {
  int docid;
  int count;
} counter_t;

typedef struct wordmap {
  char * word;
  queue_t * doclist;
} wordmap_t;

static index_t* indexBuild(char* pageDirectory);
static void indexPage(index_t* index, webpage_t* page, int id);

void printCounts(void * cp){
  counter_t* c = (counter_t*)cp;
	if (c==NULL){
		printf("doclist is empty.");
		return;
	}
  printf("docID=%d, count=%d\n", c->docid, c->count);
  return;
}

void accessQueues(void * wmp){
  wordmap_t* wm = (wordmap_t*)wmp;
  if (wm==NULL){
		printf("wordmap is empty.");
		return;
	}
  qapply(wm->doclist, printCounts);
  return;
}

bool wordSearch(void* elementp, const void* searchkeyp) {
  printf("IN WORDSEARCH: %s\n", (const char*)searchkeyp);
  wordmap_t* wordmap = (wordmap_t*)elementp;
  printf("address of wordmap: %p\n", wordmap);
  printf("wordmap key: %s\n", wordmap->word);
  return strcmp(wordmap->word, (const char*)searchkeyp) == 0;
}

bool queueSearch(void* elementp, const void* searchkeyp) {    
  printf("In queueSearch.\n");
  counter_t* counter = (counter_t*) elementp;
  int searchint = *(int*)searchkeyp;
  return (counter->docid == searchint);
}

char* normalizeWord(char* word){
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
    if ( (isalpha(c)) == 0 ){                                                          
      return NULL;                                                                     
    }                                                                                  
  }                                                                                    
  // Check if word  is uppercase                                                       
  for (cCounter = 0; cCounter < wordLen; cCounter++){                                 
    c = tolower(word[cCounter]);                                                       
    word[cCounter] = c;                                                                
  }                                                                                    
  return word;                                                                         
}

int main(int argc, char * argv[]){

    index_t* index = indexBuild("pages-depth3");

    happly(index, accessQueues);

    return 0;
}

static index_t* indexBuild(char* pageDirectory){
    index_t* index = index_new(550); //between 300 and 900 slots 
    int id = 3;

    // char filename[50];
    // sprintf(filename, "../%s/%d", pageDirectory, id);

    // FILE * loadedFile;
    // loadedFile = fopen(filename, "r");

    webpage_t* page = pageload(id, pageDirectory);

    indexPage(index, page, id);

    //webpage_delete(page);
    return index;
}

void indexPage(index_t* index, webpage_t* page, int id){
    int pos = 0;
    int c = 0;
    char *word = NULL;
    int* idp = &id;
    queue_t* queue;
    counter_t* elem;
    wordmap_t* wmap;
    wordmap_t* wordmap;
    counter_t* counter;
    
    //pos = webpage_getNextWord(page, pos, &word);
    while ((pos = webpage_getNextWord(page, pos, &word)) > 0){
        if (normalizeWord(word) != NULL){
            c++;
            printf("%s\n", word);

            if ((wmap = (wordmap_t*)(hsearch((hashtable_t *)index, wordSearch, word, strlen(word)))) != NULL) {
              //hsearch returns a wordMap that has the word as it's key.
              printf("FOUND in index.\n");
              ////qsearch the queue of wordMaps for the word.
                //returns the wordMap structure
              if((elem = (counter_t*)(qsearch(wmap->doclist, queueSearch, idp))) != NULL){
                elem->count += 1;
              }
                //take the wordMaps queue and do another qsearch for docID
                  //will return counter structure
                    //then ++ to that element.
              printf("Succeeded hsearch and qsearch.\n");
            } else { //the word is not in the index yet
              printf("NOT FOUND in index yet: %s\n", word);
              //open queue
              queue = qopen();
              //create counter with id and count
              counter = (counter_t *) malloc(sizeof(counter_t)+1);
              counter->docid = id;
              counter->count = 1;
              //add counter to the doclist
              qput(queue, counter);
              //create new wordMap
              wordmap = (wordmap_t *) malloc(sizeof(wordmap_t)+1);
              //add queue to the wordMap
              wordmap->word = malloc(strlen(word)+1);
              strcpy(wordmap->word, word);
              wordmap->doclist = queue;
              printf("wordmap key: %s\n", wordmap->word);
              //hput wordMap
              hput(index, wordmap, word, strlen(word));
              printf("address of wordmap: %p\n", wordmap);
            }
        }
        free(word);
    }
    printf("%d\n", c);
}