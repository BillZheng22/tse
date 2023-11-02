/*
 * query.c --- "querier" module
 *
 * Author: Bill Zheng, Daniel Jeon, Walker Ball, Dhruv Chandra
 * Created: 10-24-2023
 * Version: 1.0
 *
 * Description:
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <webpage.h>
#include <queue.h>
#include <hash.h>
#include <pageio.h>
#include <ctype.h>
#include <index.h>
#include <indexio.h>

/* Local helper function that checks if an inputted string is empty and only contains letters*/
static bool isValid(char* str) {
    
    // If input is less than 1 char, it's empty 
    if (strlen(str) < 1) {
        printf("[invalid query]: Empty input.\n");
        return false;
    } else {
        bool isSpace = true;
        bool isAlpha = true;

        // Iterates through every character in str and checks if it is white spce
        for (int i = 0; i < strlen(str); i++) {

            char c = str[i];
            if (!isspace(c)) {
                // str is not entirely whitespace
                isSpace = false;
                if (!isalpha(c)) {
                    isAlpha = false;
                }
            }
            
            // Non-alphabetical input
            if (!isAlpha) {
                printf("[invalid query]: Input is non-alphabetical.\n");
                return false;
            }
        }
        // str is entirely whitespace
        if (!isSpace) {
            return true;
        } else {
            printf("[invalid query]: Empty input.\n");
            return false;
        }
    }
}

int main(){
    char input[5000]; // Define a character array to store the line

    printf("> ");
    while (!feof(stdin) && fgets(input, sizeof(input), stdin)) {
        // Remove the newline character at the end of the line, if it exists
        size_t len = strlen(input);
        if (len > 0 && input[len - 1] == '\n') {
            input[len - 1] = '\0';
        }

        if(isValid(input)){
            //normalize all words?
            for (int i = 0; i < strlen(input); i++) {
                input[i] = tolower(input[i]);
            }
            printf("%s\n",input);
        }

        printf("> ");
    }
    return 0;
}