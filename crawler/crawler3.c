//crawler.c: implements and runs modules used to crawl a page and save wepages accordingly

//Author: Jake Lotreck 
//Date: 1/31/23
//Lab 4: TSE Crawler, CS 50, Winter 2023

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "../libcs50/webpage.h"
#include "../libcs50/bag.h"
#include "../libcs50/hashtable.h"
#include "../libcs50/mem.h"
#include "../common/pagedir.h"
#include "../libcs50/file.h"

int main(const int argc, char* argv[]);
static void parseArgs(const int argc, char* argv[],char** seedURL, char** pageDirectory, int* maxDepth);
static void crawl(char* seedURL, char* pageDirectory, const int maxDepth);
static void pageScan(webpage_t* page, bag_t* pagesToCrawl, hashtable_t* pagesSeen);


int main(const int argc, char* argv[]){
    parseArgs(argc, argv, &argv[1], &argv[2], (int*)argv[3]);
    crawl(normalizeURL(argv[1]),argv[2],atoi(argv[3]));
    return 0;
}
/*
 * Checks if there are the correct number of arguments and if the arguments given are valid
 */
static void parseArgs(const int argc, char* argv[], char** seedURL, char** pageDirectory, int* maxDepth){
    if (argc != 4){
        fprintf(stderr, "Invalid input: Please provide 3 agruments: seedURL, pageDirectory, maxDepth\n");
        exit(1);
    }
    //for seedURL, normalize the URL and validate it is an internal URL
    char* normalizedURL = normalizeURL(argv[1]);

    if (!isInternalURL(normalizedURL)){ //checks if internal and maxdepth is in range
        fprintf(stderr, "URl is not internal\n");
        mem_free(normalizedURL);
        exit(2); //exits 2 if URL problem
    }
    mem_free(normalizedURL); //allocate memory for URL if valid
    if (atoi(argv[3]) > 10 || atoi(argv[3]) < 0){ // for maxDepth, ensure it is an integer in specified range
        fprintf(stderr, "maxDepth is out of range\n");
        exit(3);//exits 3 if maxDepth problem
    }
    if (!pagedir_init(argv[2])){// for pageDirectory, call pagedir_init()
        fprintf(stderr, "cannot write page directory\n");
        exit(4); //exits 4 is page directory problem
    } 
     
}

/*
 * Crawls from the seedURL until maxDepth and saves pages in pageDirectory
 * Based off of pseudocode
 */
static void crawl(char* seedURL, char* pageDirectory, const int maxDepth){
   
    hashtable_t* pagesSeen = hashtable_new(200); //200 as specified in implementation

    if (pagesSeen == NULL){
        fprintf(stderr, "hashtable memory allocation error\n");
        exit(5); // exits 5 if hashtable problem
    }

    hashtable_insert(pagesSeen, seedURL, ""); //store "" as item as specified in directions
    bag_t* pagesToCrawl = bag_new(); //makes bag for uncrawled pages

    if (pagesToCrawl == NULL){ //error check for bag
        fprintf(stderr, "bag memory allocation error\n");
        exit(6); // exits 6 if bag memory error
    }

    webpage_t* first_URL;
    first_URL = webpage_new(seedURL, 0, NULL);
    bag_insert(pagesToCrawl, first_URL); //adds first webpage to bag
    
    
    int page_id = 1; //identifies page
    webpage_t* curr_page;
    while ((curr_page = bag_extract(pagesToCrawl)) != NULL){ //checks if pages are left in bag 
        if (webpage_fetch(curr_page)){
            fprintf(stdout, "%i\tFetched: %s\n", webpage_getDepth(curr_page), webpage_getURL(curr_page)); //logging
            pagedir_save(curr_page, pageDirectory, page_id);
            page_id ++; //adds 1 to id number
            if (webpage_getDepth(curr_page) < maxDepth){
                fprintf(stdout, "%i\tScanning: %s\n", webpage_getDepth(curr_page), webpage_getURL(curr_page)); //logging
                pageScan(curr_page, pagesToCrawl, pagesSeen);
            }
        }
        webpage_delete(curr_page); //delete current webpage
    }
    hashtable_delete(pagesSeen, NULL); //delete hashtable
    bag_delete(pagesToCrawl, webpage_delete); //deletes bag
}


/*
 * Given a webpage, scan the given page to extract any links (URLs), 
 * ignoring non-internal URLs; for any URL not already seen before (i.e., not in the hashtable),
 * add the URL to both the hashtable pagesSeen and to the bag pagesToCrawl
 *
 * Based off of pseudocode
 */
static void pageScan(webpage_t* page, bag_t* pagesToCrawl, hashtable_t* pagesSeen){
    
    char* curr_URL;
    int position = 0;

    while ((curr_URL = webpage_getNextURL(page, &position)) != NULL){ // while there is another URL in the page
        if (isInternalURL(curr_URL)) { // 	checks if URL is Internal
            if ((hashtable_insert(pagesSeen, curr_URL, ""))){ // inserts webpage into hashtable and checks if it worked
                webpage_t* webpage = webpage_new(curr_URL,webpage_getDepth(page) + 1,NULL);//creates a webpage
                fprintf(stdout, "%i\tFound: %s\n", webpage_getDepth(webpage), curr_URL); //logging

                bag_insert(pagesToCrawl, webpage); // inserts webpage into bag
                fprintf(stdout, "%i\tAdded: %s\n", webpage_getDepth(webpage), curr_URL); // logging
            }
            else{ //if hashtable returns false then URL has already been seen 
                fprintf(stdout, "%i\tDuplicateURL: %s\n", webpage_getDepth(page) + 1, curr_URL);
                mem_free(curr_URL); //frees URL
            }
        }
        else { //if URL is external 
            fprintf(stdout, "%i\tExtrnalURL: %s\n", webpage_getDepth(page) + 1, curr_URL);
            mem_free(curr_URL); //frees URL
        }
    }   
}
