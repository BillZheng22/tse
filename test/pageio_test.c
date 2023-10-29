/* 
 * pageio_test.c --- 
 * 
 * Author: Bill Zheng
 * Created: 10-26-2023
 * Version: 1.0
 * 
 * Description: 
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pageio.h"
#include "webpage.h"

int main() {
    char directory[] = "pages0";  // Directory where the pages are saved
    int pageID = 1;              // Page ID to load and save

    // Load the original webpage (1)
    webpage_t *originalPage = pageload(pageID, directory);

    if (originalPage != NULL) {
        printf("Original webpage loaded successfully.\n");

        // Save the original webpage
        if (pagesave(originalPage, pageID+1, directory) == 0) {
            printf("Webpage saved successfully.\n");

			//printf("ID:%d\n",pageID);
			//printf("Directory:%s\n",directory);
            // Load the saved webpage (1)
            webpage_t *savedPage = pageload(pageID+1, directory);

            if (savedPage != NULL) {
                printf("Saved webpage loaded successfully.\n");

                // Compare the loaded saved webpage with the original
                if (strcmp(webpage_getURL(originalPage), webpage_getURL(savedPage)) == 0 &&
                    webpage_getDepth(originalPage) == webpage_getDepth(savedPage) &&
                    webpage_getHTMLlen(originalPage) == webpage_getHTMLlen(savedPage) &&
                    strcmp(webpage_getHTML(originalPage), webpage_getHTML(savedPage)) == 0) {
                    printf("Original webpage matches the saved webpage.\n");
                } else {
                    printf("Original webpage does not match the saved webpage.\n");
                }

                // Clean up
                free(savedPage);
            } else {
                printf("Failed to load the saved webpage (1).\n");
            }

            // Clean up
            //pagesave(originalPage, pageID, directory); // Remove the saved file
        } else {
            printf("Failed to save the webpage (1).\n");
        }

        // Clean up
        free(originalPage);
    } else {
        printf("Failed to load the original webpage (1).\n");
    }

    return 0;
}
