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

        // Iterates through every character in str and checks if it is white space
        int i;
        for (i = 0; i < strlen(str); i++) {

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

//checks the valid query for ANDs and ORs
static bool queryCheck(char* str){
    //printf("in queryCheck.\n");
    bool prevOr = false;
    bool prevAnd = false;  

    char* query = malloc(strlen(str)+1);
    strcpy(query, str);

    int wc = 0;

    char * qword = strtok(query, " ");
    // loop through the string to extract all other tokens
    while(qword != NULL) {
        wc++;
        qword = strtok(NULL, " ");
    }
    free(query);


    //printf("WC: %d\n", wc);
    int i;
    char* word = strtok(str, " ");
    // Iterates through array
    for (i = 0; i < wc; i++) {
        //printf("Index: %d\n", i);
        //printf("WORD IS: %s\n", word);

        // If the current word is 'or'
        if (strcmp(word, "or") == 0) {
            //printf("isOR.\n");
            
            if ((i == 0) || (prevOr) || (prevAnd) || (i == (wc-1))){
                //printf("wrong OR.\n");
                return false;
            }

            prevOr = true;

        // If the current word is 'and'
        } else if (strcmp(word, "and") == 0){
            //printf("isAND.\n");
            if ((i == 0) || (prevOr) || (prevAnd) || (i == (wc-1))){
                //printf("wrong AND.\n");
                return false;
            }
            prevAnd = true;
        } else {
            prevOr = false;
            prevAnd = false;
        }
        word = strtok(NULL, " ");
    }
    //printf("Good AND OR query.\n");
    return true;
}

static void processInput(index_t* index, char* query) {
    //might need to clean the query to make sure there's only 1 space between each normalized word.
    //int numWords = 0;
    
    int id;

    wordmap_t* wmap;
    counter_t* elemc;
    int* idp;
    //int count = 0;
    int minCount = 10000000;

    int numDocs = 7; //hard coded
    char resetQuery[10000];
    strcpy(resetQuery, query);

    for (id = 1; id < numDocs+1; id++){
        idp = &id;
        strcpy(query, resetQuery);
        printf("DocID: %d -- ", id);

        // Extract the first token
        char * word = strtok(query, " ");
        // loop through the string to extract all other tokens
        while(word != NULL) {
            if ((wmap = (wordmap_t *)(hsearch((hashtable_t *)index, iwordSearch, word, strlen(word)))) != NULL){
                //if the word exists in the index, look into specific documents.

                //iterate through doclist queue of counters within wmap.

                if ((elemc = (counter_t *)(qsearch(wmap->doclist, iqueueSearch, idp))) != NULL){
                    minCount = min(minCount, elemc->count);
                    printf("%s:%d ", word, elemc->count);
                } else {
                    //if the word doesn't exist in the index, do something depending on AND or OR.
                    minCount = 0;
                    printf("%s:0 ", word);
                }
            } else {
                //if the word doesn't exist in the index, do something depending on AND or OR.
                minCount = 0;
                printf("%s:0 ", word);
            }
            
            word = strtok(NULL, " ");
        }
        
        //print everything at the very end?
        //if score is 0, print nothing.
        printf("-- %d\n", minCount);

    }
}

//need another method to check if word is less than 3 characters and if it is AND or OR

int main(){
    char input[5000]; // Define a character array to store the line
    index_t* index = indexload("pages1", "index1");

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

            char* q = malloc(strlen(input)+1);
            strcpy(q, input);
            //check the input again for ANDs and ORs
            if(queryCheck(q)){
                //for each valid input, process the input
                printf("%s\n",input);
                processInput(index, input);
            } else {
                printf("[invalid query]\n");
            }
            free(q);
        }

        printf("> ");
    }
    return 0;
}
