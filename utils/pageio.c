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
        return 1; // Return an error code
    }

    char * pageURL;
    fscanf(loadedFile, "%s", pageURL);

    webpage_t * webpage;
    
    webpage = webpage_new(pageURL, 0, NULL);

    fclose(loadedFile);
    return webpage;
}
