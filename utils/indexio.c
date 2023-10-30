/* 
 * indexio.c --- 
 * 
 * Author: Bill Zheng, Walker Ball, Daniel Jeon, Dhruv Chandra
 * Created: 
 * Version: 1.0
 * 
 * Description: 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <unistd.h>
//#include <counter.h>
#include "index.h"
#include "indexio.h"

void counterPrint(void * cp){
  counter_t* c = (counter_t*)cp;
	if (c==NULL){
		printf("doclist is empty.");
		return;
	}
  printf("docID=%d, count=%d\n", c->docid, c->count);
  return;
}

void mapPrint(void * wmp){
  wordmap_t* wm = (wordmap_t*)wmp;
  if (wm==NULL){
		printf("wordmap is empty.");
		return;
	}
  qapply(wm->doclist, printCounts);
  return;
}

int32_t indexsave(index_t* index, char *dirnm){
    FILE * savedFile;
    char filename[50];
    sprintf(filename, "../%s/indexnm", dirnm);
    savedFile = fopen(filename, "w");

    if(savedFile != NULL){
        fprintf(resultFile, "%s\n%d\n%d\n%s", webpage_getURL(pagep), webpage_getDepth(pagep), webpage_getHTMLlen(pagep), webpage_getHTML(pagep));
		    fclose(resultFile);
        return 0;
    }
    else{
        return 1;
    }

//     fprintf(fp, " ");//puts a space after key 
//     fprintf(fp, "%i %i", key, count);

}

index_t *indexload(char *dirnm){

}
