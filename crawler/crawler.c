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

			if (IsInternalURL(result)){
				printf("Internal ");
			}
			else{
				printf("External ");
			}
			printf("URL: %s\n", result);
			free(result);
		}
		// Deallocate the webpage
		webpage_delete(page);
		exit(EXIT_SUCCESS);
	} else {
		exit(EXIT_FAILURE);
	}                 
	return 0;
}
