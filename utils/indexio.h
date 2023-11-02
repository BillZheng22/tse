#pragma once
/* 
 * indexio.h --- 
 * 
 * Author: Bill Zheng
 * 
 * Description: 
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <unistd.h>
#include <index.h>

bool iwordSearch(void *elementp, const void *searchkeyp);

bool iqueueSearch(void *elementp, const void *searchkeyp);

int32_t indexsave(index_t* index, char *dirnm, char *indexnm);

index_t *indexload(char *dirnm, char *indexnm);

