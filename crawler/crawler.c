/* 
 * crawler.c --- 
 * 
 * Author: William H. Zheng
 * Created: 10-15-2023
 * Version: 1.0
 * 
 * Description: 
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "webpage.h"

int main(){
	int pos;
	bool fetchResult;
	char *seedURL, *html, *result;
	webpage_t *page;

	printf("Hello World!\n");

	seedURL = "https://thayer.github.io/engs50/";
	page = webpage_new(seedURL, 0, NULL);

	// Fetch the webpage HTML
	fetchResult = webpage_fetch(page);

	if (fetchResult) {
		// Fetch succeeded, print the HTML
		html = webpage_getHTML(page);
		//		printf("Webpage HTML:\n%s\n", html);
		pos = 0;                
	
		while ((pos = webpage_getNextURL(page, pos, &result)) > 0) {
			//printf("URL: %s (%s)\n", result, isInternalURL(result, baseURL) ? "Internal" : "External");
			printf("URL: %s", result);
			if (IsInternalURL(result) == 0){
				printf("Internal\n");
			}
			else{
				printf("External\n");
			}
			//  free(result);
		}
		// Deallocate the webpage
		webpage_delete(page);
		return EXIT_SUCCESS;
	} else {
		exit(EXIT_FAILURE);
	}                 
	return 0;
}
