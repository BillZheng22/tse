/*
 * query.c --- "querier" module
 *
 * Author: Bill Zheng, Daniel Jeon, Walker Ball, Dhruv Chandra
 * Created: 10-24-2023
 * Version: 1.0
 *
 * Description:
 *
 */

#include <queue.h>
#include <hash.h>
#include <ctype.h>
#include <index.h>
#include <indexio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

static bool isValid(char* str);
static void processInput(index_t* index, char* query);
int min(int a, int b);
bool iwordSearch(void *elementp, const void *searchkeyp);
bool iqueueSearch(void *elementp, const void *searchkeyp);


// standard min function
int min(int a, int b) {
    return (a < b) ? a : b;
}

/* Local helper function that checks if an inputted string is empty and only contains letters*/
static bool isValid(char* str) {
    
    // If input is less than 1 char, it's empty 
    if (strlen(str) < 1) {
        printf("[invalid query]: Empty input.\n");
        return false;
    } else {
        bool isSpace = true;
        bool isAlpha = true;

        // Iterates through every character in str and checks if it is white spce
        for (int i = 0; i < strlen(str); i++) {

            char c = str[i];
            if (!isspace(c)) {
                // str is not entirely whitespace
                isSpace = false;
                if (!isalpha(c)) {
                    isAlpha = false;
                }
            }
            
            // Non-alphabetical input
            if (!isAlpha) {
                printf("[invalid query]: Input is non-alphabetical.\n");
                return false;
            }
        }
        // str is entirely whitespace
        if (!isSpace) {
            return true;
        } else {
            printf("[invalid query]: Empty input.\n");
            return false;
        }
    }
}

static void processInput(index_t* index, char* query) {
    //might need to clean the query to make sure there's only 1 space between each normalized word.
    //int numWords = 0;
    
    int id = 1;

    wordmap_t* wmap;
    counter_t* elemc;
    int* idp = &id;
    //int count = 0;
    int minCount = 10000000;

    // Extract the first token
    char * word = strtok(query, " ");
    // loop through the string to extract all other tokens
    while(word != NULL) {
        //numWords++;
        
        if ((wmap = (wordmap_t *)(hsearch((hashtable_t *)index, iwordSearch, word, strlen(word)))) != NULL){
            if ((elemc = (counter_t *)(qsearch(wmap->doclist, iqueueSearch, idp))) != NULL){
                minCount = min(minCount, elemc->count);
                printf("%s:%d ", word, elemc->count);
            }
        } else {
            minCount = 0;
            printf("%s:0 ", word);
        }
        
        word = strtok(NULL, " ");
    }
    
    //char** words = splitString(query, &numWords);


    // int i;
    // for (i = 0; i < numWords; i++){
    //     char* word = words[i];

    //     if ((wmap = (wordmap_t *)(hsearch((hashtable_t *)index, iwordSearch, word, strlen(word)))) != NULL){
    //         if ((elemc = (counter_t *)(qsearch(wmap->doclist, iqueueSearch, idp))) != NULL){
    //             minCount = min(minCount, elemc->count);
    //             printf("%s:%d ", word, elemc->count);
    //         }
    //     } else {
    //         minCount = 0;
    //         printf("%s:0 ", word);
    //     }
    // }
    printf(" -- %d\n", minCount);
}

//process input function:
//looks for word in the index, gets the wordmap
//from the wordmap, get counter
//from the counter, get the docID and then the count

int main(){
    char input[5000]; // Define a character array to store the line
    index_t* index = indexload("pages0", "index0");

    printf("> ");
    while (!feof(stdin) && fgets(input, sizeof(input), stdin)) {
        // Remove the newline character at the end of the line, if it exists
        size_t len = strlen(input);
        if (len > 0 && input[len - 1] == '\n') {
            input[len - 1] = '\0';
        }

        if(isValid(input)){
            //normalize all words?
            for (int i = 0; i < strlen(input); i++) {
                input[i] = tolower(input[i]);
            }
            printf("%s\n",input);

            //for each input, process the input, make a new function
            processInput(index, input);
        }

        printf("> ");
    }
    return 0;
}
