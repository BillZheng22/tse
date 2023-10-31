/* 
 * pageio.c --- saving and loading crawler webpage files
 * 
 * Author: Bill Zheng, Walker Ball, Daniel Jeon, Dhruv Chandra
 * Created: 
 * Version: 1.0
 * 
 * Description: pagesave saves an existing webpage to a file with a
 * numbered name (e.g. 1,2,3 etc); pageload creates a new page by
 * loading a numbered file. For pagesave, the directory must exist and
 * be writable; for loadpage it must be readable.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <unistd.h>
#include <webpage.h>
#include <pageio.h>
/*
 * pagesave -- save the page in filename id in directory dirnm
 *
 * returns: 0 for success; nonzero otherwise
 *
 * The suggested format for the file is:
 *   <url>
 *   <depth>
 *   <html-length>
 *   <html>
 */
int32_t pagesave(webpage_t *pagep, int id, char *dirnm){
  FILE * resultFile;
  char filename[50];

  sprintf(filename, "../%s/%d", dirnm, id);
  resultFile = fopen(filename, "w");

  if(resultFile != NULL){
    fprintf(resultFile, "%s\n%d\n%d\n%s", webpage_getURL(pagep), webpage_getDepth(pagep), webpage_getHTMLlen(pagep), webpage_getHTML(pagep));
		fclose(resultFile);
    return 0;
  }
  else{
    return 1;
  }
}


/* 
 * pageload -- loads the numbered filename <id> in direcory <dirnm>
 * into a new webpage
 *
 * returns: non-NULL for success; NULL otherwise
 */
webpage_t *pageload(int id, char *dirnm){
    FILE * loadedFile;
    char filename[50];
    sprintf(filename, "../%s/%d", dirnm, id);
    loadedFile = fopen(filename, "r");
    if (loadedFile == NULL) {
        perror("Error opening file");
        return NULL; // Return an error code
    }

    char url[1024]; // Adjust the size as needed for your URL format
    int depth;
    int htmlLen;

    if (fscanf(loadedFile, "%1023s", url) != 1 ||
        fscanf(loadedFile, "%d", &depth) != 1 ||
        fscanf(loadedFile, "%d", &htmlLen) != 1) {
          perror("Loaded file wrong.");
          fclose(loadedFile);
          return NULL;
    }

    char* urlp = malloc(sizeof(url) + 1);
    urlp = strcpy(urlp, url);

    int i;
    int c;

    c = fgetc(loadedFile);

    char html[2*htmlLen+1];
    for(i = 0; i < htmlLen; i++){
        c = fgetc(loadedFile);
        html[i] = c;
    }

    // Null-terminate the string
    html[htmlLen] = '\0';

    char* htmlp = malloc(sizeof(html)+1); //malloc HTML
    htmlp = strcpy(htmlp, html);

    // Close the file
    fclose(loadedFile);

    // Don't forget to free the memory when you're done with 'html'
    webpage_t *page = webpage_new(urlp, depth, htmlp);

    free(urlp);
    //free(htmlp);

    return page;
}
