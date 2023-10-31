/*
* indexer.c - Reads document files produced by
* TSE crawler, builds an index, and writes that index to a file
* 
* Jake Lotreck, Dartmouth CS 50, October 2021
*/

#include <stdio.h>
#include <stdlib.h>
#include <file.h>
#include "../common/index.h"
#include "../common/pagedir.h"
#include "../common/word.h"
#include "../libcs50/webpage.h"
#include "../libcs50/counters.h"
#include "../libcs50/mem.h"


static void parseArgs(const int argc, char* argv[]);
static index_t* indexBuild(char* pageDirectory);
static void indexPage(index_t* index, webpage_t* page, int id);


int main(int argc, char* argv[]){
    parseArgs(argc, argv);
    index_t* index = indexBuild(argv[1]);

    FILE* fp = fopen(argv[2], "w"); //checked if it was able to be opened in parse args

    index_save(index, fp);
    index_delete(index); //memory problems 

    fclose(fp); //closes file

    return 0;
}

static void parseArgs(const int argc, char* argv[]){
    FILE* fp;

    if (argc != 3){
        fprintf(stderr, "Please enter two arguments: first the page directory and then the index filename");
        exit(1); // 1 for file errors
    }
    if (!pagedir_validate(argv[1])){
        fprintf(stderr, "Page Directory is invalid");
        exit(2);// 2 for page dir errors 
    }
    if ((fp = fopen(argv[2], "w")) == NULL){
        fprintf(stderr, "filename is not writeable\n");
        exit(3); // 3 for file writing issues
    }
    fclose(fp);
}

static index_t* indexBuild(char* pageDirectory){
    index_t* index = index_new(550); //between 300 and 900 slots 
    int id = 1;
    FILE* fp;
    char* filename = (char*)mem_malloc(strlen(pageDirectory) + 12); //allows up to 10 digits in file 
    sprintf(filename,"%s/%d",pageDirectory,id);
    
    webpage_t* page;

    while ((fp=fopen(filename,"r"))!=NULL){ //null check 
        page = pagedir_load(fp); // Loads a webpage from the document file 'pageDirectory/id'
        if (page != NULL){ // if null do nothing
            indexPage(index, page, id);
            id++;
            webpage_delete(page);
            fclose(fp);
            sprintf(filename, "%s/%i", pageDirectory, id); 
        }
    }
    mem_free(filename);
    return index;
}
void indexPage(index_t* index, webpage_t* page, int id){
    int pos = 0;
    char* curr_word;
    char* normalized;
    counters_t* counter;

    while((curr_word = webpage_getNextWord(page, &pos)) != NULL){ //loops through words in webpage till there are non left 
        if (strlen(curr_word)>2){ //skips trivial words less than 3 characters 
            normalized = normalize_word(curr_word); //turns letters to lowercase
            if ((counter = index_find(index, normalized)) != NULL){ // finds word in the index
                counters_add(counter, id);
            }
            else{ // if work doesnt already have a counter in index 
                counter = counters_new();
                counters_add(counter,id);
                index_insert(index, curr_word, counter);
            }
        }
        mem_free(curr_word); //mem leak
    }
}