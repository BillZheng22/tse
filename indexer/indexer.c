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

typedef struct wordmap {
  char * word; 
  queue_t * doclist;
} wordmap_t;

typedef struct counter {
  char * docid; 
  int count;
} counter_t;

bool matchingWordsSearch(void* elementp, const void* searchkeyp) {                     
  wordmap_t *wordmap = (wordmap_t*) elementp;                                 
  const char *key = (const char*) searchkeyp;                                          
  if (elementp == NULL) {                                                               
    return false;                                                                      
  }
  if (strcmp(wordmap->word, key) == 0) {
    return true;
  }
  else {
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
  for ( cCounter = 0; cCounter < wordLen; cCounter++){                                 
    c = word[cCounter];                                                                
    if ( (isalpha(c)) == 0 ){                                                          
      return NULL;                                                                     
    }                                                                                  
  }                                                                                    
  // Check if word  is uppercase                                                       
  for ( cCounter = 0; cCounter < wordLen; cCounter++){                                 
    c = tolower(word[cCounter]);                                                       
    word[cCounter] = c;                                                                
  }                                                                                    
  return word;                                                                         
}

int main(int argc, char * argv[]){

    printf("Hello!\n");

    webpage_t *page = pageload(1, "pages0");

    hashtable_t *index = hopen(100); // change to argv[1] later

    int pos = 0;
    char *word = NULL;
    
    while ((pos = webpage_getNextWord(page, pos, &word)) > 0) {
        if (normalizeWord(word) != NULL){
            printf("%s\n", word);

            wordmap_t * input = (wordmap_t *) malloc(sizeof(wordmap_t));
            input->word = word;

            hput(index, input, input->word, strlen(input->word));
            if (hsearch(index, matchingWordsSearch, input->word, strlen(input->word))!= NULL) {
              input->doclist
            }


            free(word);
        } else {
            free(word);
        }
        pos++;
    }

    //webpage_delete(page);
    free(page);

    return 0;
}
