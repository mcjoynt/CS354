////////////////////////////////////////////////////////////////////////////////
// Main File:        check_queens.c
// This File:        check_queens.c
// Other Files:      n/a
// Semester:         CS 354 SPRING 2019
//
// Author:           Matthew McJoynt
// Email:            mmcjoynt@wisc.edu
// CS Login:         mcjoynt
//
/////////////////////////// OTHER SOURCES OF HELP //////////////////////////////
//                   fully acknowledge and credit all sources of help,
//                   other than Instructors and TAs.
//
// Persons:          n/a
//
// Online sources:   n/a
//
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

char *COMMA = ",";

/* COMPLETED:
 * Retrieves from the first line of the input file,
 * the number of rows and columns for the board.
 *
 * fp: file pointer for input file
 * rows: pointer to number of rows
 * cols: pointer to number of columns
 */
void get_dimensions(FILE *fp, int *rows, int *cols) {
        char *line = NULL;
        size_t len = 0;
        if (getline(&line, &len, fp) == -1) {
                printf("Error in reading the file\n");
                exit(1);
        }

        char *token = NULL;
        token = strtok(line, COMMA);
        *rows = atoi(token);

        token = strtok(NULL, COMMA);
        *cols = atoi(token);
}


/*
 * Returns 1 if and only if there exists at least one pair
 * of queens that can attack each other.
 * Otherwise returns 0.
 *
 * board: heap allocated 2D board
 * rows: number of rows
 * cols: number of columns
 */
int check_queens(int **board, int rows, int cols) {

	//check rows
	bool queen = false;
	for(int i = 0; i < rows; i++) {
		queen = false;
		for(int j = 0; j < cols; j++) {
			if(*(*(board + i) + j) == 1) {
				if(queen == false){
					queen = true;
				} else {
					return 1;
				}
			}
		}
	}

	//check cols
	for(int j = 0; j < cols; j++) {
		queen = false;
		for(int i = 0; i < rows; i++) {
			if(*(*(board + i) + j) == 1) {
				if(queen == false){
					queen = true;
				} else {
					return 1;
				}
			}
		}
	}

	//check diagonals top left to bottom right
	for(int i = 0; i < rows; i++) {
		queen = false;
		for(int j = 0; j < cols; j++) {
			if((i + j) < rows){
				if(*(*(board + i + j) + j) == 1){
					if(queen == true) {
						return 1;
					} else {
						queen = true;
					}
				}
			}
		}
	}

	//check diagonals bottom left to top right
	for(int i = 0; i < rows; i++) {
		queen = false;
		for(int j = 0; j < cols; j++) {
			if((i - j) < rows && (i - j) >= 0) {
				if(*(*(board + i - j) + j) == 1){
					if(queen == true) {
						return 1;
					} else {
						queen = true;
					}
				}
			}
		}
	}

	return 0;
}


/*
 * This program prints true if the input file has any pair
 * of queens that can attack each other, and false otherwise
 *
 * argc: CLA count
 * argv: CLA value
 */
int main(int argc, char *argv[]) {

        //Check if number of command-line arguments is correct.

        if(argc != 2){
		printf("Incorrect number of command-line arguments");
		exit(1);
	}


	//Open the file and check if it opened successfully.
        FILE *fp = fopen(*(argv + 1), "r");
        if (fp == NULL) {
                printf("Cannot open file for reading\n");
                exit(1);
        }


        //Declare local variables.
        int rows, cols;


        //Call get_dimensions to retrieve the board dimensions.
	get_dimensions(fp, &rows, &cols);

        //Dynamically allocate a 2D array of dimensions retrieved above.
        int **m;
	m = malloc(sizeof(int*) * rows);
	for(int i = 0; i < rows; i++) {
		m[i] = malloc(sizeof(int*) * cols);
	}

        //Read the file line by line.
        //Tokenize each line wrt comma to store the values in your 2D array.
        char *line = NULL;
        size_t len = 0;
        char *token = NULL;
        for (int i = 0; i < rows; i++) {

                if (getline(&line, &len, fp) == -1) {
                        printf("Error while reading the file\n");
                        exit(1);
                }

                token = strtok(line, COMMA);
                for (int j = 0; j < cols; j++) {
                        //Complete the line of code below
                        //to initialize your 2D array.
                        *(*(m + i) + j) = atoi(token);
                        token = strtok(NULL, COMMA);
                }
        }

        //Call the function check_queens and print the appropriate
        //output depending on the function's return value.
        if(check_queens(m, rows, cols) == 1){
		printf("true\n");
	} else{
		printf("false\n");
	}

        //Free all dynamically allocated memory.
	for(int i = 0; i < rows; i++) {
		free(*(m + i));
	}

	free(m);

        //Close the file.
        if (fclose(fp) != 0) {
                printf("Error while closing the file\n");
                exit(1);
        }

        return 0;
}
