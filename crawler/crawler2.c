/*
* crawler.c: implementation of tse crawler
* the crawler goes through a hyperlinked webpage and retrieves each of the webpages, up to a certain depth  
*
* Author: Isabelle Han 
* Date: Oct. 11, 2023 
* Fall 2023 CS50 Lab 4
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../libcs50/webpage.h"
#include "../libcs50/bag.h"
#include "../libcs50/hashtable.h"
#include "../common/pagedir.h"

/********************************FUNCTION PROTOTYPES*****************************************************/
static void parseArgs(const int argc, char* argv[], char** seedURL, char** pageDirectory, int* maxDepth);
static void crawl(char* seedURL, char* pageDirectory, const int maxDepth);
static void pageScan(webpage_t* page, bag_t* pagesToCrawl, hashtable_t* pagesSeen);

int bagSize = 0; // keeps track of the number of items in the bag

/* 
* Main function - parses arguments and crawls 
* Exit code 1: null parameters
*/
int main(const int argc, char* argv[])
{   
    char* url = NULL; 
    char* pageDir = NULL; 
    int depth = -1; // initialize depth to impossible value 

    if (argc != 4){ // invalid number of arguments 
        fprintf(stderr, "Usage: %s [seedURL] [pageDirectory] [maxDepth]\n", argv[0]);
        exit(1); 
    } 
    // parse arguments 
    parseArgs(argc, argv, &url, &pageDir, &depth); 
    
    if (!url || !pageDir || depth < 0){
        if (!url){ 
            free(url); 
        }
        exit(1); // unable to crawl, invalid parameters
    } else{ // crawl 
        crawl(url, pageDir, depth); 
    }

    exit(0); 
}

/* 
* Given arguments from the command line, extract them into the function parameters; return only if successful.
* 
* We return: 
*   true if arguments are correctly parsed, else false
*   for seedURL, normalize the URL and validate it is an internal URL
*   for pageDirectory, call pagedir_init()
*   for maxDepth, ensure it is an integer in specified range
* 
* User provides: 
*   arguments entered from the command line
*/
static void parseArgs(const int argc, char* argv[], char** seedURL, char** pageDirectory, int* maxDepth)
{   
    // get and store the the seedURL 
    // normalize seedURL
    char* normalized = normalizeURL(argv[1]); 
    *seedURL = normalized; 
    
    if (!seedURL){ 
        // error normalizing the URL 
        fprintf(stderr, "Error: unable to normalize URL\n");
        return; 
    } else if (!isInternalURL(normalized)){ // verify that it is an internal URL
        fprintf(stderr, "Error: not an internal URL\n"); 
        // free the string 
        normalized = NULL; 
        *seedURL = NULL; 
        free(normalized); 
        return; 
    } 
    
    // get pageDirectory 
    *pageDirectory = argv[2]; 
    // try to make a directory 
    if(!(pagedir_init(*pageDirectory))){ // unable to initialize a directory 
        //set seedURL to NULL 
        normalized = NULL; 
        *seedURL = NULL; 
        free(normalized); 
        fprintf(stderr, "Error: failed to initialize .crawler file in directory\n"); 
        return; 

    }

    // get maxDepth 
    char space = ' '; 
    // store command line argumetn as an integer 
    if (sscanf(argv[3], "%d%c", maxDepth, &space) != 1){
        // unsuccessfuly assigned maxDepth 
        *maxDepth = -1; 
        free(normalized); 
        fprintf(stderr, "Error: invalid max depth\n"); 
        return; 
    } 

    // ensure maxDepth is an integer in the specified range [0,10]
    if (*maxDepth < 0 || *maxDepth > 10){
        fprintf(stderr, "Error: max depth is not within range\n"); 
        *maxDepth = -1; 
        free(normalized); 
        return;   
    }
    //successfully parsed args
    return; 
} 


/* 
* Do the real work of crawling from seedURL to maxDepth and saving pages in pageDirectory
* 
* We return: 
*   void 
* Caller provides: 
*   valid seedURL, valid and exisiting pageDirectory, valid int maxDepth 
* 
* Exit code 2: error allocating memory
* Exit code 4: unable to insert elements into data structures 
* 
*/
static void crawl(char* seedURL, char* pageDirectory, const int maxDepth)
{
    // create a webpage out of the seed URL
    webpage_t* seedpage = webpage_new(seedURL, 0, NULL);

    // initialize the bag and add a webpage representing the seedURL at depth 0
    bag_t* toVisit = bag_new(); 
    if (!toVisit){ // error initializing the bag
        fprintf(stderr, "Error: unable to allocate memory for bag \n"); 
        exit(2); 
    }
    
    // set up the hashtable to keep track of visited URLS
    hashtable_t* visited = hashtable_new(200); 
    if (!visited){ // there was an error initializing the hashtable 
        fprintf(stderr, "Error: unable to alloacte memory for hashtable\n"); 
        exit(2); 
    }

    bag_insert(toVisit, seedpage); 
    bagSize = 1; 
    
    // add seedURL to hashtable 
    if (!hashtable_insert(visited, webpage_getURL(seedpage), "")){ // insert into hashtable
        // error inserting into the hashtable 
        hashtable_delete(visited, NULL); 
        exit(4); 

    }
    int id = 1;         // keeps track of the id of each webpage that you visit to create output files
    while (bagSize > 0){ 
        webpage_t* curr = (webpage_t*)bag_extract(toVisit); // while there are still URLs left to visit 
        bagSize -= 1;
        if (curr){
            // fetch the HTML for that webpage
            if (webpage_fetch(curr)){
                //try to save to a new directory
                pagedir_save(curr, pageDirectory, id);
                id++; 
                
                if (webpage_getDepth(curr) < maxDepth){//    if the webpage is not at maxDepth,
                    //pageScan that HTML
                    pageScan(curr, toVisit, visited); 
                }

                // delete that webpage
                webpage_delete(curr);

            } else { // unsuccessful fetch 
                webpage_delete(curr);  
                continue; 
            }
        
        } else{ // curr is null, unsuccessfully fetched curr
            webpage_delete(curr);  
            continue; 
            
        }
    }
    // delete the hashtable if not null
    if (visited != NULL){
        hashtable_delete(visited, NULL);
    } 

    // delete the bag if not null 
    if (toVisit != NULL){
    bag_delete(toVisit, NULL); 
    }
}


/* 
* This function implements the pagescanner mentioned in the design. Given a webpage, 
* scan the given page to extract any links (URLs), ignoring non-internal URLs; for any URL not already seen before 
* i.e., not in the hashtable), add the URL to both the hashtable pagesSeen and to the bag pagesToCrawl.
* 
*/
static void pageScan(webpage_t* page, bag_t* pagesToCrawl, hashtable_t* pagesSeen)
{
    int pos = 0;  // keeps track of where you are in the html of the page 
    char* next; 
    // while there is another URL in the page
    while ((next = webpage_getNextURL(page, &pos))){
        char* nextURL = normalizeURL(next); 
        // check if that URL is Internal
        if (isInternalURL(nextURL)){ 
             if (hashtable_insert(pagesSeen, nextURL, "")){ 
                // create a webpage_t for it
                webpage_t* newPage = webpage_new(nextURL, webpage_getDepth(page) + 1 , NULL); 
                if (newPage){
                // insert the webpage into the bag
                bag_insert(pagesToCrawl, newPage); 
                bagSize += 1; 
                } 
            } else{ 
                free(nextURL); 
            }
            
        } else { 
            // if it is not internal, we do not want to add it to our bag
            free(nextURL); 
        }
        free(next); 
    } 
    hashtable_insert(pagesSeen, webpage_getURL(page), "");
}