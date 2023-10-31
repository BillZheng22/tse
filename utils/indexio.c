/* 
 * indexio.c --- 
 * 
 * Author: Bill Zheng, Walker Ball, Daniel Jeon, Dhruv Chandra
 * Created: 
 * Version: 1.0
 * 
 * Description: 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <unistd.h>
#include <queue.h>
#include <hash.h>
#include <index.h>
#include <indexio.h>

FILE* globalFile = NULL;

void counterPrint(void * cp){
  counter_t* c = (counter_t*)cp;
	if (c==NULL){
		printf("doclist is empty.");
		return;
	}
  fprintf(globalFile, "%d %d ", c->docid, c->count);
  return;
}

void mapPrint(void * wmp){
  wordmap_t* wm = (wordmap_t*)wmp;
  if (wm==NULL){
		printf("wordmap is empty.");
		return;
	}
  fprintf(globalFile, "%s ", wm->word);

  qapply(wm->doclist, counterPrint);

  fprintf(globalFile, "\n");
  return;
}

bool iwordSearch(void* elementp, const void* searchkeyp) {
  printf("IN WORDSEARCH: %s\n", (const char*)searchkeyp);
  wordmap_t* wordmap = (wordmap_t*)elementp;
//  printf("address of wordmap: %p\n", wordmap);
  printf("wordmap key: %s\n", wordmap->word);
  return strcmp(wordmap->word, (const char*)searchkeyp) == 0;
}

bool iqueueSearch(void* elementp, const void* searchkeyp) {    
  printf("In queueSearch.\n");
  counter_t* counter = (counter_t*) elementp;
  int searchint = *(int*)searchkeyp;
  return (counter->docid == searchint);
}

int32_t indexsave(index_t* index, char *dirnm){
    //FILE * savedFile;
    char filename[50];
    sprintf(filename, "../%s/indexnm", dirnm);
    globalFile = fopen(filename, "w");
    if(globalFile != NULL){
        happly(index, mapPrint);
        fclose(globalFile);
        return 0;
    }else{
        return 1;
    }
}

index_t *indexload(char *dirnm){
    FILE * globalFile = NULL;
    int id;
    int count;
    char* word = NULL;
    char w[100];
    char c;

    int* idp = &id;
    queue_t* queue;
    counter_t* elemc;
    wordmap_t* wmap;
    wordmap_t* wordmap;
    counter_t* counter;

    char delimiter[50] = " ";

    index_t* index = index_new(550);

    char filename[50];
    sprintf(filename, "../%s/indexnm", dirnm);

    fflush(stdout);
    printf("%s\n", filename);
    fflush(stdout);
    globalFile = fopen(filename, "r");
    
    if (globalFile == NULL) {
        perror("Error opening file");
        return 1;
    }
    fflush(stdout);
    printf("Got here.\n");
    fflush(stdout);

/*
    fflush(stdout);
    printf("address of globalFile: %p\n", globalFile);
    fflush(stdout);

*/

    /*
          if (fscanf(globalFile, "%c", &c) == NULL) {
      printf("Error when reading word from file\n");
      exit(3);
    }
    fflush(stdout);
    printf("%c\n", c);
    fflush(stdout);
    */
    
    // if (fscanf(globalFile, "%99s", w) == 1){
    //   printf("%s", w);
    // } else {
    //       fprintf(stderr, "Error when reading word from file\n");
    //       exit(3); //3 is word loading error
    // }

    while (!(feof(globalFile))){ //end of file check
        printf("Entered file.\n");
        // word = strtok(NULL, delimiter);
        // printf("%s", word);
        if (fscanf(globalFile, "%s", w) == 1){
          fflush(stdout);
          printf("%s\n", w);
          fflush(stdout);
          word = malloc((strlen(w)+1)*sizeof(char));
          strcpy(word, w);

          while ((fscanf(globalFile, "%d %d ", &id, &count)) == 2) {
            if ((wmap = (wordmap_t*)(hsearch((hashtable_t *)index, iwordSearch, word, strlen(word)))) != NULL) {
              printf("FOUND in index.\n");
              if((elemc = (counter_t*)(qsearch(wmap->doclist, iqueueSearch, idp))) != NULL){
                elemc->count = count;
              } else {
                //put new counter into the doclist queue
                //needs to have the new id and a count of 1.
                counter = (counter_t *) malloc(sizeof(counter_t)+1);
                counter->docid = id;
                counter->count = count;
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
              counter->count = count;
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
              // printf("address of wordmap: %p\n", wordmap);
            }
          }
        } else {
          fprintf(stderr, "Error when reading word from file\n");
          exit(3); //3 is word loading error
        }
        free(word); //frees word each time after use
    }
    fclose(globalFile);
    return index;
}
