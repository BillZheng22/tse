/* 
 * crawler.c --- 
 * 
 * Author: Bill Zheng, Walker Ball, Dhruv Chandra, Daniel Jeon
 * Created: 10-15-2023
 * Version: 1.0
 * 
 * Description: 
 * 
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <webpage.h>
#include <queue.h>
#include <hash.h>

int main(const int argc, char* argv[]);
//static void parseArgs(const int argc, char* argv[],char** seedURL, char** pageDirectory, int* maxDepth);
static void crawl(char* seedURL, char* pageDirectory, const int maxDepth);
static void pageScan(webpage_t* page, queue_t* pagesToCrawl, hashtable_t* visitedURLs);

int queueSize = 0; // keeps track of the number of items in the bag

bool searchURL(void *elementp, const void *key){
  char *url = (char *)elementp;
  return(strcmp(url, (char *) key) == 0);
}

int32_t pagesave(webpage_t *pagep, int id, char *dirname){
  FILE * resultFile;
  char filename[50];

  sprintf(filename, "../%s/%d", dirname, id);
  resultFile = fopen(filename, "w");

  if(resultFile != NULL){
    fprintf(resultFile, "%s \n %d \n %d \n %s", webpage_getURL(pagep), webpage_getDepth(pagep), webpage_getHTMLlen(pagep), webpage_getHTML(pagep));
		fclose(resultFile);
    return 0;
  }
  else{
    return 1;
  }
}

int main(int argc, char * argv[]){
  crawl(argv[1],argv[2],atoi(argv[3]));
  return 0;
}

/*
 * Crawls from the seedURL until maxDepth and saves pages in pageDirectory
 * Based off of pseudocode
 */
static void crawl(char* seedURL, char* pageDirectory, const int maxDepth){
   
    hashtable_t* visitedURLs = hopen(5);

    if (visitedURLs == NULL){
        fprintf(stderr, "hashtable memory allocation error\n");
        exit(5); // exits 5 if hashtable problem
    }

    queue_t* pagesToCrawl = qopen(); //makes queue for pages that need to be crawled

    if (pagesToCrawl == NULL){ //error check for queue
        fprintf(stderr, "bag memory allocation error\n");
        exit(6); // exits 6 if queue memory error
    }

    // if (!hput(visitedURLs, webpage_getURL(seedpage), )){ // insert into hashtable
    //     // error inserting into the hashtable 
    //     hashtable_delete(visited, NULL); 
    //     exit(4);
    // }


    webpage_t* firstURL;
    firstURL = webpage_new(seedURL, 0, NULL);
    qput(pagesToCrawl, firstURL); //adds first webpage to queue
    queueSize = 1;
    hput(visitedURLs, webpage_getURL(firstURL), webpage_getURL(firstURL), strlen(webpage_getURL(firstURL))); //put firstURL into the hashtable

    int pageID = 1;
    webpage_t* currPage;

    while(queueSize > 0){
      currPage = (webpage_t*)qget(pagesToCrawl);
      queueSize -= 1;
      if (webpage_fetch(currPage)){
          fprintf(stdout, "%i\tFetched: %s\n", webpage_getDepth(currPage), webpage_getURL(currPage)); //logging
          //IMPLEMENT PAGE SAVE
          pagesave(currPage, pageID, pageDirectory);
          pageID ++; //adds 1 to id number
          if (webpage_getDepth(currPage) < maxDepth){
            fprintf(stdout, "%i\tScanning: %s\n", webpage_getDepth(currPage), webpage_getURL(currPage)); //logging
            pageScan(currPage, pagesToCrawl, visitedURLs);
          }
      }
      webpage_delete(currPage); //delete current webpage
      fprintf(stdout, "queueSize: %i\n",queueSize);
    }
    fprintf(stdout, "Finished while loop.\n");

    if(visitedURLs != NULL){
      fprintf(stdout, "visitedURLs is not NULL.\n");
      hclose(visitedURLs); //delete hashtable
    }
    fprintf(stdout, "Finished hclose.\n");
    if(pagesToCrawl != NULL){
      fprintf(stdout, "pagesToCrawl is not NULL.\n");
      qclose(pagesToCrawl); //deletes queue
    }
    fprintf(stdout, "Finished qclose.\n");
}

/*
 * Given a webpage, scan the given page to extract any links (URLs), 
 * ignoring non-internal URLs; for any URL not already seen before (i.e., not in the hashtable),
 * add the URL to both the hashtable pagesSeen and to the bag pagesToCrawl
 *
 * Based off of pseudocode
 */
static void pageScan(webpage_t* page, queue_t* pagesToCrawl, hashtable_t* visitedURLs){
    char* result;
    int pos = 0;

    while ((pos = webpage_getNextURL(page, pos, &result)) > 0){ // while there is another URL in the page
        if (IsInternalURL(result)) { // 	checks if URL is Internal
        //BEFORE PUTTING MUST CHECK IF IT'S ALREADY INSIDE
          if (hsearch(visitedURLs, searchURL, result, strlen(result)) == NULL){
            if ((hput(visitedURLs, result, result, strlen(result))) == 0){ // inserts URL into hashtable and checks if it worked
                webpage_t* webpage = webpage_new(result, webpage_getDepth(page) + 1, NULL);//creates a webpage
                fprintf(stdout, "%i\tFound: %s\n", webpage_getDepth(webpage), result); //logging
                if(webpage){
                  qput(pagesToCrawl, webpage); // inserts webpage into queue
                  queueSize += 1;
                  fprintf(stdout, "%i\tAdded: %s\n", webpage_getDepth(webpage), result); // logging
                }
            }
            else{ //if hashtable returns false then URL has already been seen
              fprintf(stdout, "%i\tDuplicate1URL: %s\n", webpage_getDepth(page) + 1, result);
              free(result); //frees URL
            }
          }
          else { //if hashtable returns false then URL has already been seen
            fprintf(stdout, "%i\tDuplicate2URL: %s\n", webpage_getDepth(page) + 1, result);
            free(result); //frees URL
          }
        }
        else { //if URL is external 
            fprintf(stdout, "%i\tExternalURL: %s\n", webpage_getDepth(page) + 1, result);
            free(result); //frees URL
        }
    }
    //hput(visitedURLs, webpage_getURL(page), webpage_getURL(page), strlen(webpage_getURL(page)));
}