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
#include <indexio.h>

// typedef struct counter {
//   int docid;
//   int count;
// } counter_t;

// typedef struct wordmap {
//   char * word;
//   queue_t * doclist;
// } wordmap_t;

int total = 0;
int end = 0;

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
  //printf("IN WORDSEARCH: %s\n", (const char*)searchkeyp);
  wordmap_t* wordmap = (wordmap_t*)elementp;
  //printf("address of wordmap: %p\n", wordmap);
  printf("wordmap key: %s\n", wordmap->word);
  return strcmp(wordmap->word, (const char*)searchkeyp) == 0;
}

bool queueSearch(void* elementp, const void* searchkeyp) {    
  //printf("In queueSearch.\n");
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

    end = atoi(argv[1]);
    printf("END: %d", end);

    index_t* index = indexBuild("pages2");

    //happly(index, accessQueues);

    indexsave(index, "pages2");

    //index = indexload("pages-depth3");

    //indexsave(index, "pages0");

    return 0;
}

static index_t* indexBuild(char* pageDirectory){
    index_t* index = index_new(900); //between 300 and 900 slots 
    int id = 1;

    FILE * loadedFile;
    char filename[50];
    sprintf(filename, "../%s/%d", pageDirectory, id);

    webpage_t* page;

    while ((loadedFile = fopen(filename,"r")) != NULL){ //null check 
        page = pageload(id, pageDirectory); // Loads a webpage from the document file 'pageDirectory/id'
        if (page != NULL){ // if null do nothing
            indexPage(index, page, id);
            id++;
            if(id > end){
              printf("breaking at: %d", id);
              break;
            }
            //webpage_delete(page);
            fclose(loadedFile);
            sprintf(filename, "../%s/%d", pageDirectory, id);
            printf("new File name: %s\n", filename); 
        }
    }
    return index;
}

void indexPage(index_t* index, webpage_t* page, int id){
    int pos = 0;
    // int c = 0;
    char *word = NULL;
    int* idp = &id;
    queue_t* queue;
    counter_t* elemc;
    wordmap_t* wmap;
    wordmap_t* wordmap;
    counter_t* counter;
    
    //pos = webpage_getNextWord(page, pos, &word);
    while ((pos = webpage_getNextWord(page, pos, &word)) > 0){
        printf("%s\n", word);
        if (normalizeWord(word) != NULL){
            total++;
            printf("%s\n", word);

            if ((wmap = (wordmap_t*)(hsearch((hashtable_t *)index, wordSearch, word, strlen(word)))) != NULL) {
              printf("FOUND in index.\n");
              if((elemc = (counter_t*)(qsearch(wmap->doclist, queueSearch, idp))) != NULL){
                elemc->count += 1;
              } else {
                //put new counter into the doclist queue
                //needs to have the new id and a count of 1.
                counter = (counter_t *) malloc(sizeof(counter_t)+1);
                counter->docid = id;
                counter->count = 1;
                qput(wmap->doclist, counter);
              }
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
              //printf("address of wordmap: %p\n", wordmap);
            }
        }
        free(word);
    }
    printf("TOTAL WORDS: %d\n", total);
}