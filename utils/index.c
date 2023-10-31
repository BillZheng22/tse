/*
* index.c: module providing the data structure to represent the 
* in-memory index, and functions to read and write index files 
*
* Author: Bill Zheng
* Date: 10/29/2023
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <queue.h>
#include <hash.h>
#include <webpage.h>
#include <pageio.h>

/*******Global Variables*******/
typedef hashtable_t index_t;

typedef struct counter {
  int docid;
  int count;
} counter_t;

typedef struct wordmap {
  char * word;
  queue_t * doclist;
} wordmap_t;

void wordmap_delete(void* elemp){
  wordmap_t* wm = (wordmap_t*) elemp;
  free(wm->word);
  qclose(wm->doclist);
}

/*******index_new()**********/
index_t* index_new(int numSlots) {
    index_t* index = hopen(numSlots);
    return index;
}
/*******index_delete()**********/
void index_delete(index_t* index){
    happly(index, wordmap_delete);
    hclose(index);
}

/*******index_set()**********/
// bool index_set(index_t* index, char* word, int docID, int count){

// }

/*******index_insert()**********/
// bool index_insert(index_t* index, char* word, void* item){ //the item is the queue of 
//     return (hput(index,item,word,strlen(word)) == 0);
// }



/*******index_find()**********/
// counters_t* index_find(index_t* index, char* key){
//     counters_t* counter_found = hashtable_find(index,key);
//     return counter_found;
// }


/*******deletefunc()**********/
// void delete_func(void* item){
//   counters_delete((counters_t*)item);
// }

/*******iterate_helper()**********/
// void iterate_helper(void* fp, const int key, int count){
//     fprintf(fp, " ");//puts a space after key 
//     fprintf(fp, "%i %i", key, count);
// }

/*******index_iterate()**********/
// void index_iterate(void* fp, const char* key, void* item){
//     fprintf(fp, "%s", key);
//     fprintf(fp, " "); //puts a space after key 
//     counters_iterate(item, fp, iterate_helper);
//     fprintf(fp, "\n");

// }


//EDIT FILE POINTER

/*******index_save()**********/
// void index_save(index_t* index, FILE* fp){
//     hashtable_iterate(index, fp, index_iterate);
// }

// /*******index_load()**********/
// void index_load(index_t* index, FILE* fp){
//     int id;
//     int count;
//     counters_t* curr_counter;

//     while (!(feof(fp))){ //end of file check
//         char* word = file_readWord(fp);

//         if (word == NULL) {
//             fprintf(stderr, "Error when reading word from file\n");
//             exit(3); //3 is word loading error
//         }
//         else{
//             while ((fscanf(fp, "%d %d ", &id, &count)) == 2) {
//                 if ((curr_counter = index_find(index, word)) != NULL) {
//                     counters_set(curr_counter, id, count); //updates values for id and count in previous counter
//                 }
//                 else{
//                     curr_counter = counters_new();
//                     counters_set(curr_counter, id, count);
//                     index_insert(index,word,curr_counter);
//                 }
//             }
//         }
//         free(word); //frees word each time after use
//     }
// }














