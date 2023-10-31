/* 
 * indexio_test.c --- 
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
#include <webpage.h>
#include <queue.h>
#include <hash.h>
#include <index.h>
#include "indexio.h"

int main() {
    index_t* index = indexload("pages-depth3", "indexnm");

    indexsave(index, "pages0", "indexnm0");
    
    return 0;
}
