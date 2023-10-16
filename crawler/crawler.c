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
#include "webpage.h"

int main(){
	int fetchResult, pos;
	char *seedURL, *html, *result;

	printf("Hello World!\n");

	seedURL = "https://thayer.github.io/engs50/";
  webpage_new(seedURL, 0, NULL);
	
  // Fetch the webpage HTML
	fetchResult = webpage_fetch(page);   

  if (fetchResult == 0) {
		
		// Fetch succeeded, print the HTML
    html = webpage_getHTML(page);
    printf("Webpage HTML:\n%s\n", html);

		pos = 0;                

    while ((pos = webpage_getNextURL(page, pos, &result)) > 0) {

			//printf("URL: %s (%s)\n", result, isInternalURL(result, baseURL) ? "Internal" : "External");
			
      printf("URL: %s", result);
      if (isInternalURL(result, seedURL) == 0){
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
	}
	else {
		exit(EXIT_FAILURE);
	}                 
	return 0;
}
