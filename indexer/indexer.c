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

bool wordSearch(void* elementp, const void* searchkeyp) {                     
  //queue_t* queuep = (queue_t*) elementp;
  if (elementp != NULL) {
    return true;
  } else {
    return false;
  }
}

bool queueSearch(void* elementp, const void* searchkeyp) {    
  printf("In queueSearch.\n");                 
  counter_t* counter = (counter_t*) elementp;
  int searchint = *(int*)searchkeyp;
  if (counter->docid == searchint) {
    printf("Finished queueSearch.\n");
    return true;
  } else {
    printf("Finished queueSearch.\n");
    return false;
  }
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

    printf("Hello!\n");

    index_t* index = indexBuild("pages0");

    return 0;
}

static index_t* indexBuild(char* pageDirectory){
    index_t* index = index_new(550); //between 300 and 900 slots 
    int id = 1;

    // char filename[50];
    // sprintf(filename, "../%s/%d", pageDirectory, id);

    // FILE * loadedFile;
    // loadedFile = fopen(filename, "r");

    webpage_t* page = pageload(id, pageDirectory);

    indexPage(index, page, id);

    webpage_delete(page);
    return index;
}

void indexPage(index_t* index, webpage_t* page, int id){
    int pos = 0;
    char *word = NULL;
    int* idp = &id;
    
    while ((pos = webpage_getNextWord(page, pos, &word)) > 0) {
        if (normalizeWord(word) != NULL){
            printf("%s\n", word);
            queue_t* queue;
            counter_t* elem;

            if (queue = (queue_t*)(hsearch(index, wordSearch, word, strlen(word))) != NULL) {
              //hsearch returns a queue of wordMaps

              //qsearch the queue of wordMaps for the word.
                //returns the wordMap structure
                //take the wordMaps queue and do another qsearch for docID
                  //will return counter structure
                    //then ++ to that element.

              if(queue){
                printf("%p",&queue);
              }
              fflush(stdout);
              printf("Finished hsearch.\n");
              fflush(stdout);
              if(elem = (counter_t*)(qsearch(queue, queueSearch, idp)) != NULL){
                fflush(stdout);
                printf("Finished qsearch.\n");
                fflush(stdout);
                elem->count += 1;
              }
              printf("Succeeded hsearch and qsearch.\n");
            } else { //the word is not in the index yet
              //open queue
              //create counter with id and count
              //add counter to the doclist
              //create new wordMap
              //add queue to a wordMap
              //hput wordMap

              printf("word is not in index yet.\n");
              queue = qopen();
              counter_t* counter = (counter_t *) malloc(sizeof(counter_t));
              counter->docid = id;
              counter->count = 1;
              qput(queue, counter);
              hput(index, queue, word, strlen(word));
            }
        }
        free(word);
        pos++;
    }
}