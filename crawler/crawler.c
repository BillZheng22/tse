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
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <webpage.h>
#include <queue.h>
#include <hash.h>

#define MAX_VISITED_URLS 1000

void printURL(void *data){

	fflush(stdout);
	printf("Inside printURL function.\n");
	fflush(stdout);
	
	webpage_t *page = (webpage_t *)data;
	char *url = webpage_getURL(page);
	//	webpage_delete((void *)url);
	printf("Internal URL: %s\n", url);
}

bool searchURL(void *elementp, const void *key){

	fflush(stdout);
	printf("Inside searchURL function.\n");
	fflush(stdout);

	char *url = webpage_getURL((webpage_t *)elementp);
	// webpage_delete((void *)url);
	int cmp;

	cmp = strcmp(url, (char *)key);

	free(url);
	
	if (cmp != 0){
		return false;
	} 
	return true;
	
	//	return (strcmp(url, (char *)key) == 0);
}

int main(){
	int pos = 0;
	bool fetchResult;
	char *seedURL, *result;
	//	char *html;
	webpage_t *page;
	hashtable_t *visited;
	queue_t* pageQueue;
	
	seedURL = "https://thayer.github.io/engs50/";
	page = webpage_new(seedURL, 0, NULL);

	// Fetch the webpage HTML
	fetchResult = webpage_fetch(page);

	visited = hopen(100);
	
	pageQueue = qopen();
	fflush(stdout);
	printf("Start\n");
	fflush(stdout);
	if (fetchResult) {
		// Fetch succeeded, print the HTML
		//		html = webpage_getHTML(page);
		//		printf("Webpage HTML:\n%s\n", html);
		while ((pos = webpage_getNextURL(page, pos, &result)) > 0) {

			if (IsInternalURL(result)){
				// Create a new webpage for the internal URL
				if (hsearch(visited, searchURL, result, strlen(result)) == NULL){
					webpage_t *internalPage = webpage_new(result, 0, NULL);
					qput(pageQueue, internalPage);
					hput(visited, internalPage, result, strlen(result));
				}
			}
			//else{
				//				printf("External ");
			//}
			//printf("URL: %s\n", result);
			fflush(stdout);
			printf("While once\n");
			fflush(stdout);
			free(result);
		}
		fflush(stdout);
		printf("Visited Hash URLs:\n");
		fflush(stdout);
		happly(visited, printURL);
		
		hclose(visited);
		webpage_delete(page);
		exit(EXIT_SUCCESS);
	} else {
		hclose(visited);
		webpage_delete(page);
		exit(EXIT_FAILURE);
	}
}
