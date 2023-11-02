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
    index_t* index = indexload("pages-depth3", "FINAL");

    indexsave(index, "pages2", "FINAL");

    index = indexload("pages2", "FINAL");

    indexsave(index, "pages2", "FINALCHECK");


    const char *file1_path = "../pages2/FINAL";
    const char *file2_path = "../pages2/FINALCHECK";

    // Open the two files for reading
    FILE *file1 = fopen(file1_path, "r");
    FILE *file2 = fopen(file2_path, "r");

    if (file1 == NULL || file2 == NULL) {
        perror("Error opening files");
        exit(EXIT_FAILURE);
    }

    char line1[256];
    char line2[256];
    int line_number = 0;

    // Read and compare lines
    while (fgets(line1, sizeof(line1), file1) != NULL && fgets(line2, sizeof(line2), file2) != NULL) {
        line_number++;

        // Compare the lines and print differences
        if (strcmp(line1, line2) != 0) {
            printf("Difference at line %d:\n", line_number);
            printf("File 1: %s", line1);
            printf("File 2: %s", line2);
        }
    }

    // Check for differences in line count
    if (fgets(line1, sizeof(line1), file1) != NULL) {
        printf("File 1 has extra lines:\n");
        while (fgets(line1, sizeof(line1), file1) != NULL) {
            printf("File 1: %s", line1);
        }
    } else if (fgets(line2, sizeof(line2), file2) != NULL) {
        printf("File 2 has extra lines:\n");
        while (fgets(line2, sizeof(line2), file2) != NULL) {
            printf("File 2: %s", line2);
        }
    } else {
        printf("The two files (indexes) are the exact same.\n");
    }

    // Close the files
    fclose(file1);
    fclose(file2);
    
    return 0;
}
