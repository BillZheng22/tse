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
#include "index.h"

int32_t indexsave(index_t* index, char *dirnm);

index_t *indexload(char *dirnm);

