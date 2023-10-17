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
#include <stdbool.h>
#include <webpage.h>
#include <queue.h>
#include <hash.h>

void printURL(void *data){
	webpage_t *page = (webpage_t *)data;
	char *url = webpage_getURL(page);
	webpage_delete(url);
	printf("Internal URL: %s\n", url);
}

int main(){
	int pos;
	bool fetchResult;
	char *seedURL, *html, *result;
	webpage_t *page;
	queue_t* pageQueue;
	
	seedURL = "https://thayer.github.io/engs50/";
	page = webpage_new(seedURL, 0, NULL);

	// Fetch the webpage HTML
	fetchResult = webpage_fetch(page);

	pageQueue = qopen();
	
	if (fetchResult) {
		// Fetch succeeded, print the HTML
		html = webpage_getHTML(page);
		//		printf("Webpage HTML:\n%s\n", html);
		pos = 0;                
	
		while ((pos = webpage_getNextURL(page, pos, &result)) > 0) {

			if (IsInternalURL(result)){
				// Create a new webpage for the internal URL

				webpage_t *internalPage = webpage_new(result, 0, NULL);
				
				qput(pageQueue, internalPage);
				//printf("Internal ");
			}
			//else{
				//				printf("External ");
			//}
			//printf("URL: %s\n", result);
			free(result);
		}
		//		free(result);
	} else {
		free(result);
		qclose(pageQueue);
		return EXIT_FAILURE;
	}
	qapply(pageQueue, printURL);
	qclose(pageQueue);
	webpage_delete(page);

	return EXIT_SUCCESS;
}
