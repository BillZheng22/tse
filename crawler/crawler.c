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
#include <unistd.h>

#define _POSIX_SOURCE
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

int32_t pagesave(webpage_t *pagep, int id, char *dirname) {
	char idname[100];
	char path[100];

	sprintf(idname, "%d" ,id);

	strcpy(path, "../");
	strcat(path, dirname);
	strcat(path, "/");
	strcat(path, idname);

	FILE * file = fopen(path, "w");

	//printing the link
	fprintf(file, "%s\n", webpage_getURL(pagep));
	fprintf(file, "%d\n", webpage_getDepth(pagep));
	fprintf(file, "%d\n", webpage_getHTMLlen(pagep));
	fprintf(file, "%s\n", webpage_getHTML(pagep));
	fclose(file);

	//check to see if file exists
	if (access(path, F_OK) != 0) {
		printf("Error: cannot access file.");
		return -1;
	}
	else {
		printf("File Access: ");
		if (access(path, F_OK) == 0) printf("read ");
		if (access(path, R_OK) == 0) printf("write ");
		if (access(path, W_OK) == 0) printf("execute ");
		printf("\n");
		return 0;
	}
}

int main(){
	int pos = 0;
	bool fetchResult;
	char *seedURL, *result;
	//	char *html;
	webpage_t *page;
	webpage_t *temp; //for breadth depth
	hashtable_t *visited;
	queue_t* pageQueue;
	webpage_t * internalPage; //internal page

	seedURL = "https://thayer.github.io/engs50/";
	page = webpage_new(seedURL, 0, NULL);

	// Fetch the webpage HTML
	fetchResult = webpage_fetch(page);

	//opening the hash table that stores all the visited webpages
	visited = hopen(100);
	
	//queue of all pages
	pageQueue = qopen();
	fflush(stdout);
	printf("Start\n");
	fflush(stdout);

	//breadth depth search method
	if (fetchResult) {
		// Fetch succeeded, print the HTML
		//		html = webpage_getHTML(page);
		//		printf("Webpage HTML:\n%s\n", html);
		qput(pageQueue, page);
		temp = qget(pageQueue);
		while ((pos = webpage_getNextURL(temp, pos, &result)) > 0) {
			//if internal url exists
			//printf("%s\n", result);
			if (IsInternalURL(result)){
				// Create a new webpage for the internal URL if the webpage has not been visited
				if (hsearch(visited, searchURL, result, strlen(result)) == NULL){
					internalPage = webpage_new(result, 0, NULL);
					//put the new page in the queue
					qput(pageQueue, internalPage);
					//mark page as visited
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

		pagesave(page, 1, "pages");
		webpage_delete(page);
		exit(EXIT_SUCCESS);
	} else {
		hclose(visited);
		webpage_delete(page);
		exit(EXIT_FAILURE);
	}
}
