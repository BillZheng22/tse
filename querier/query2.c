/*
 * query2.c - 'querier' module
 * Author:
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <unistd.h>
#include "webpage.h"
#include "counters.h"
#include "index.h"
#include "word.h"
#include "../common/pagedir.h"

/**************** file-local global variables ****************/
/* none */

/**************** global types ****************/
typedef struct counterpair {
    int key;
    int count;
} counterpair_t;

typedef struct counterdir {
    counters_t* ctrs;
    char* pageDirectory;
} counterdir_t;


/**************** local functions ****************/
/* not visible outside this file */
static void parseInput(int argc, char* argv[]);
static void processQuery(index_t* index, char* q, char* pageDirectory);
static bool isValid(char* str);
static char* cleanQuery(char** words, int numWords);
static char** splitString(char* str, int* numWords);
static char* createPath(char* pageDirectory, int docID);

// Iterator functions passed into counters_iterate for various purposes
static void initDocScores(void* currDocScores, const int key, const int count);
static void modifyWordCounts(void* wordCounts, const int key, const int count);
static void updateDocScores(void* currDocScores, const int key, const int count);
static void updateTotalScores(void* totDocScores, const int key, const int count);
static void rankDocScores(void* totDocScores, const int key, const int count);
static void compareScores(void* cpair, const int key, const int count);
static void getMatchCount(void* size, const int key, const int count);

/**************** functions ****************/

/* For the sake of testing whether input is from the keyboard or not */
int fileno(FILE *stream);

/* Main method that calls */
int main(const int argc, char* argv[]) {
    parseInput(argc, argv);
    return 0;
}

/**************** counterpair_new ****************/
/* Create a new counterpair object (containing a integer key and an integer character pair)
 *
 * Caller provides:
 *   Values for integer key and integer character
 * We return:
 *   pointer to the new counterpair;
 * We guarantee:
 *   counterpair object initialized with inputted values
 * Caller is responsible for:
 *   later calling counterpair_delete.
 */
counterpair_t* counterpair_new(int k, int c)
{   
    counterpair_t* cpair = malloc(sizeof(counterpair_t)); // Allocates memory
    cpair->key = k;                           // Creates key member
    cpair->count = c;                         // Creates count member
    return cpair;                             // Returns the index
}

/**************** counterpair_set ****************/
/* Sets the values of a counterpair object (with a integer key and an integer character pair)
 *
 * Caller provides:
 *   Valid pointer to counterpair object and values for integer key and integer character
 * We return:
 *   nothing
 * We guarantee:
 *   counterpair object updated with inputted values
 */
void counterpair_set(counterpair_t* cpair, int k, int c)
{   
    cpair->key = k;                           // Creates key member
    cpair->count = c;                         // Creates count member
}

/**************** counterdir_new ****************/
/* Create a new counterdir object (containing a counters set and string for a pageDirectory)
 *
 * Caller provides:
 *   Values for counters_t* pointer and string
 * We return:
 *   pointer to the new counterdir;
 * We guarantee:
 *   counterdir object initialized with inputted values
 * Caller is responsible for:
 *   later calling counterdir_delete.
 */
counterdir_t* counterdir_new(counters_t* c, char* d)
{
    counterdir_t* cdir = malloc(sizeof(counterdir_t)); // Allocates memory
    cdir->ctrs = c;
    cdir->pageDirectory = d;
    return cdir;
}

/**************** counterdir_delete ****************/
/* Deletes an inputted counterdir object 
 * Caller provides: 
 *   Valid pointer to counters and string objects
 * We guarantee:
 *   Memory allocated for the counterdir ONLY is freed
 * Caller is later reponsible for:
 *   Freeign the counters member for counterdir
 */
void counterdir_delete(counterdir_t* cdir)
{
    free(cdir);
}

/* Local function that parses input and output, determining if input is valid and if so, processing as a query*/
static void parseInput(int argc, char* argv[]) 
{   
    // Ensures received 2 keyboard arguments
    if (argc == 3) {

        // First argument is assumed to be pageDirectory
        char* pageDirectory = argv[1];

        // Second argument is assumed to be the indexFile name; uses it to create the indexFile
        char* indexFilename = argv[2];
        FILE* indexFile = fopen(indexFilename, "r");

        // If the pageDirectory is valid (contains crawler)
        if (pagedir_validate(pageDirectory)) {

            // If indexFile could be opened, initialize a new index object with its contents and begin querying
            if (indexFile != NULL) {
                index_t* index = index_load(indexFile);
                if (index != NULL) {
                    // Only print out the prompt if from keyboard input
                    if (isatty(fileno(stdin))) {
                        printf("Query: ");
                    }
                    // Creates buffer
                    char input[5000];

                    // While we can get input from stdin and not EOF
                    while (!feof(stdin) && fgets(input, sizeof(input), stdin)) {
                        // Checks if the inputted line is empty
                        if (isValid(input)) {
                            // Processes the query
                            processQuery(index, input, pageDirectory);
                        }
                        // Only reprint the prompt if from keyboard input
                        if (isatty(fileno(stdin))) {
                            printf("-----------------------------------------------\nQuery: ");
                        }
                    }
                    printf("--\n");
                    // Clean-up
                    index_delete(index);
                    fclose(indexFile);
                } else {
                    // Couldn't load indexFile into index
                    fprintf(stderr, "Couldn't load index from indexFile...\n");

                    // Clean-up
                    fclose(indexFile);
                    exit(4);
                }
            } else {
                // Couldn't open the indexFile itself
                fprintf(stderr, "Error opening indexFile...\n");
                exit(3);
            }
        } else {
            // Bad pageDirectory
            fprintf(stderr, "Invalid pageDirectory...\n");
            exit(2);
        }
        
    } else {    
        // Not enough args
        fprintf(stderr, "Invalid number of arguments...\n");
        exit(1);
    }
}

/* Function that processes a query given an index and pageDirectory*/
static void processQuery(index_t* index, char* q, char* pageDirectory) {

    // Pre-processes the query (normalizes it)
    char* buffer = malloc(strlen(q)+1);
    strcpy(buffer, q);
    char* query = normalizeWord(buffer);

    // Tokenizes the query into words
    int numWords = 0;
    char** words = splitString(query, &numWords);

    // Gets a cleaned query
    char* cQuery = cleanQuery(words, numWords);
    
    // Creates counters for total scores and the current AND expression;  booleans for whether previous word was and/or
    counters_t* totDocScores = counters_new();
    counters_t* currDocScores = NULL;

    // Creates boolean values to store if current word is 1) or, 2) and, or 3) in adjacence
    bool isOr = false;
    bool isAnd = false;  
    bool isMultiple = false;

    int i;
    // Iterates through array
    for (i = 0; i < numWords; i++) {
        char* word = words[i];

        // If the current word is 'or'
        if (strcmp(word, "or") == 0) {
            
            // If preceded by an 'or'
            if (isOr) {
                isMultiple = true;
                break;
            }

            isOr = true;

            // If at the first word or preceded by an 'and'
            if ((i == 0) || (isAnd)) {
                break;
            // Valid 'or'
            } else {
                counters_iterate(currDocScores, totDocScores, updateTotalScores);
                counters_delete(currDocScores);
                currDocScores = NULL;
            }
            
        // If the current word is 'and'
        } else if (strcmp(word, "and") == 0){

            // if preceded by an 'and'
            if (isAnd) {
                isMultiple = true;
                break;
            }
             
            isAnd = true;

            // If at first word or preceded by an 'or'
            if ((i == 0) || (isOr)) {
                break;
            }

        } else {
            // Current word is not 'or' or 'and'
            isOr = false;
            isAnd = false;

            // Finds the wordCounts for the given word within the index object
            counters_t* wordCounts = index_findWord(index, word);

            // If starting a new 'and' sequence (either to begin the query or following an 'or')
            if (currDocScores == NULL) {
                // Creates new counters to keep track of 'new' and sequence
                currDocScores = counters_new();
                if (wordCounts == NULL) {
                    // wordCounts are uninitialized, so creates wordCounts and initializes their values with the first word's counts
                    wordCounts = counters_new();
                    counters_iterate(wordCounts, currDocScores, initDocScores);
                    counters_delete(wordCounts); // Deletes wordCounts
                } else {
                    // In the same and sequence, so continue updating values in wordCounts (choosing minimum occ.s for each word)
                    counters_iterate(wordCounts, currDocScores, initDocScores);
                }
            // If startin a new 'or' sequence
            } else {
                // Same as above
                if (wordCounts == NULL) {
                    wordCounts = counters_new();
                    counters_iterate(currDocScores, wordCounts, modifyWordCounts);        
                    counters_iterate(wordCounts, currDocScores, updateDocScores);    
                    counters_delete(wordCounts);
                } else {
                    counters_iterate(currDocScores, wordCounts, modifyWordCounts);        
                    counters_iterate(wordCounts, currDocScores, updateDocScores);    
                }
            }

            // Sets wordCounts to NULL
            wordCounts = NULL;
        }
    }
    
    // If a sequence of multiple 'and's, 'or's
    if (isMultiple) {
        printf("Error: cannot have a sequence of adjacent 'and's and/or 'or's\n");
    } else if (isAnd && isOr) { // Consecutive and, or
        printf("Error: 'and' and 'or' cannot be adjacent (in any order)\n");
    } else if (isAnd) { 
        if (i==0) { // first word is and
            printf("Error: 'and' cannot be first\n");
        } else { // last word is and
            printf("Error: 'and' cannot be last\n");
        }
    } else if (isOr) {
        if (i==0) { // first word is or
            printf("Error: 'or' cannot be first\n");
        } else { // last word is or
            printf("Error: 'or' cannot be last\n");
        }
    } else { // Valid query
        printf("Searching for '%s'...\n", cQuery); // Print out query
        counters_iterate(currDocScores, totDocScores, updateTotalScores); // Update final scores

        int numMatches = 0; // Get the number of total matches at the end 
        counters_iterate(totDocScores, &numMatches, getMatchCount);
        
        // if no matches found
        if (numMatches == 0) {
            printf("No documents match.\n");
        } else { // found at least one match
            printf("\nMatches %d documents (ranked):\n", numMatches);
            counterdir_t* cdir = counterdir_new(totDocScores, pageDirectory);
            counters_iterate(totDocScores, cdir, rankDocScores);
            counterdir_delete(cdir);   
        }
    }

    // Clean-up
    for (int i = 0; i < numWords; i++) {
        free(words[i]);
    }
    free(words);
    free(query);
    free(cQuery);
    if (currDocScores != NULL) {
        counters_delete(currDocScores);
    } if (totDocScores != NULL) {
        counters_delete(totDocScores);
    }
}

/* Local function that turns a list of words from a query into a clean string, returning it*/
static char* cleanQuery(char** words, int numWords) {
    char* cQuery = NULL;
    // Iterates through the words from the query and appends them to cQuery
    for (int i = 0; i < numWords; i++) {
        char* word = words[i];
        if (cQuery == NULL) {
            cQuery = malloc(strlen(word)+1);
            strcpy(cQuery, word);
        } else {
            size_t new_len = strlen(cQuery) + strlen(word) + 2; // Add 1 for null terminator
            cQuery = realloc(cQuery, new_len);
            strcat(cQuery, " ");
            strcat(cQuery, word);
        }
    }
    
    // Returns cleaned query
    return cQuery;
}

/* Local helper function that splits a string into words*/
static char** splitString(char* str, int* numWords) {

    char** words = NULL;  // Array to hold all the words 
    char* word = NULL;  // Running current word
    int wordSize = 0;  // Running size of current word

    // Iterates through the length of the string
    for (int i = 0; i < strlen(str); i++) {
        // If the current word is not a space, allocate memory and increase the wordSize and append the char to word
        if(!isspace(str[i])) {
            word = realloc(word, wordSize+1);
            word[wordSize]=str[i];
            wordSize++;
        } else { // Otherwise, as long as the word is not NULL, add current state of word to list of words
            if ((word != NULL)) {
                word = realloc(word, wordSize+1);
                word[wordSize] = '\0';
                char* wordCopy = malloc(strlen(word)+1);
                strcpy(wordCopy, word);
                
                (*numWords)++;
                words = realloc(words, (*numWords)*sizeof(char*));
                words[(*numWords)-1] = wordCopy;

                free(word);
                wordSize = 0;
                word=NULL;
            }
        }
    }

    // Free the word if NULL
    if (word != NULL) {
        free(word);
    }

    // Return words list
    return words;
}

/* Local helper function that checks if an inputted string is empty and only contains letters*/
static bool isValid(char* str) {
    
    // If input is less than 1 char, it's empty 
    if (strlen(str) < 1) {
        printf("Empty input\n");
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
                printf("Input is non-alphabetical\n");
                return false;
            }
        }
        // str is entirely whitespace
        if (!isSpace) {
            return true;
        } else {
            printf("Empty input\n");
            return false;
        }
    }
}

/* Local function that creates the path for reading in a page's file */
static char* createPath(char* pageDirectory, int docID) {

    // Allocates memory
    char* path = malloc(strlen(pageDirectory)+12);

    // Adds the docID to pageDirectory
    sprintf(path, "%s%d", pageDirectory, docID);

    // Returns the concatenated path
    return path;
}

/**************** Series of 'itemfunc's for counters_iterate used to merge, update, and rank counters ****************/

/* itemfunc that iterates through each key=count pair in counters and uses cpair to keep track of the current max;
 * Together with compareScores, effectively implements a selection sort
 */
static void rankDocScores(void* cdir, const int key, const int count) {

    // For current iteration, keeps track of key and count in struct
    counterpair_t* cpair = counterpair_new(key, count);

    // Iteratees through the counters of total occurrences and compares the current pair with the others, ultimately leaving cpair as the key and count of the current max-occuring pair
    counters_iterate(((counterdir_t*)cdir)->ctrs, cpair, compareScores);
    
    // If the count isn't 0
    if ((cpair->count) != 0) {

        // Create a path with pageDirectory and the max-occuring docID and open the crawled file in pageDirectory
        char* path = createPath(((counterdir_t*)cdir)->pageDirectory, cpair->key);
        FILE* fp = fopen(path, "r");

        // If the file could be opened
        if (fp != NULL) {

            webpage_t* docPage = pagedir_load(fp); // loads in the crawled file as a webpage

            // Outputs the docID of the current max, its score, and URL
            printf("score:  %d doc  %d: %s", cpair->count, cpair->key, webpage_getURL(docPage));
            webpage_delete(docPage);
            
            fclose(fp); // Closes the file
        } else {
            fprintf(stderr, "Unable to open file '%s'\n",path);
            exit(5);
        }
        
        free(path); // Free the path since createPath allocates memory

    }
    
    // Sets the score of the current max to 0 so that the next max can be found on the following iteration
    counterpair_set(cpair, cpair->key, 0);
    counters_set(((counterdir_t*)cdir)->ctrs, cpair->key, 0);
    free(cpair); 
}

/* itemfunct that compares cpair's count with current key=count pair, updates its values if count exceeds its own */
static void compareScores(void* cpair, const int key, const int count) {
    if ((((counterpair_t*)cpair)->count) < count) {
        counterpair_set(cpair, key, count);
    }
}

/* itemfunc that sets the values of inputted current doc scores (for and-sequence) to wordCounts encountered */
static void initDocScores(void* currDocScores, const int key, const int count) {
    counters_set(currDocScores, key, count); 
}

/* itemfunc that sets unencountered docIds counts to 0*/
static void modifyWordCounts(void* wordCounts, const int key, const int count) {
    int wordCount = counters_get(wordCounts, key);
    if ((wordCount == 0)) {
        counters_set(wordCounts, key, wordCount);
    }
}

/* itemfunc that takes the minimum value of each docIds score for and sequence*/
static void updateDocScores(void* currDocScores, const int key, const int count) {
    int docScore = counters_get(currDocScores, key);
    if (count < docScore) {
        counters_set(currDocScores, key, count);
    }
}

/* itemfunc that takes increments the total score for each document with the value from an and sequence*/
static void updateTotalScores(void* totDocScores, const int key, const int count) {
    int totDocScore = counters_get(totDocScores, key);
    int newScore = totDocScore + count;
    counters_set(totDocScores, key, newScore);
}

/* itemfunc tht iterates through the total scores at the end and determines the number of matches in total */
static void getMatchCount(void* size, const int key, const int count) {

    if (count > 0) {
        *(int*)size+=1;
    }

}