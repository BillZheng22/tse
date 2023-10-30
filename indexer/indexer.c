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
  wordmap_t* wordmap;
  printf("IN WORDSEARCH: %s\n", (char*)searchkeyp);

  //HSEARCH PUTS THIS FUNCTION INTO ANOTHER QSEARCH 0x55c0f5814260
  wordmap = (wordmap_t*) elementp;
  printf("address of wordmap: %p\n", wordmap);
  printf("wordmap key: %s\n", wordmap->word);
  if (strcmp(wordmap->word,(char *)searchkeyp) == 0) {
    printf("TRUE wordSearch.\n");
    return true;
  } else {
    printf("FALSE wordSearch.\n");
    return false;
  }
}

bool queueSearch(void* elementp, const void* searchkeyp) {    
  printf("In queueSearch.\n");
  counter_t* counter = (counter_t*) elementp;
  int searchint = *(int*)searchkeyp;
  if (counter->docid == searchint) {
    printf("TRUE queueSearch.\n");
    return true;
  } else {
    printf("FALSE queueSearch.\n");
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
    queue_t* queue;
    counter_t* elem;
    wordmap_t* wmap;
    wordmap_t* wordmap;
    counter_t* counter;
    
    while ((pos = webpage_getNextWord(page, pos, &word)) > 0) {
        if (normalizeWord(word) != NULL){
            printf("%s\n", word);

            if ((wmap = (wordmap_t*)(hsearch((hashtable_t *)index, wordSearch, word, strlen(word)))) != NULL) {
              //hsearch returns a wordMap that has the word as it's key.
              fflush(stdout);
              printf("FOUND in index.\n");
              fflush(stdout);
              ////qsearch the queue of wordMaps for the word.
                //returns the wordMap structure
              if((elem = (counter_t*)(qsearch(wmap->doclist, queueSearch, idp))) != NULL){
                fflush(stdout);
                printf("Finished qsearch.\n");
                fflush(stdout);
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
              counter = (counter_t *) malloc(sizeof(counter_t));
              counter->docid = id;
              counter->count = 1;
              //add counter to the doclist
              qput(queue, counter);
              //create new wordMap
              wordmap = (wordmap_t *) malloc(sizeof(wordmap_t));
              //add queue to the wordMap
              wordmap->word = (char*) &word;
              wordmap->doclist = queue;
              printf("wordmap key: %s\n", wordmap->word);
              //hput wordMap
              hput(index, wordmap, word, strlen(word));
              printf("address of wordmap: %p\n", wordmap);
              //free(word);
            }
        }
        free(word);
        pos++;
    }
}