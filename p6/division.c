////////////////////////////////////////////////////////////////////////////////
// Main File: intdate.c
// This File: division.c
// Other Files: n/a
// Semester: CS 354 SPRING 2019
//
// Author: Matthew McJoynt
// Email: mmcjoynt@wisc.edu
// CS Login: mcjoynt
//
/////////////////////////// OTHER SOURCES OF HELP //////////////////////////////
// fully acknowledge and credit all sources of help,
// other than Instructors and TAs.
//
// Persons: n/a
//
// Online sources: n/a
//
////////////////////////////////////////////////////////////////////////////////


#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

int count; // global variable that keeps track of the number of successful divisions

/**
  * Handles the SIGFPE interrupt, prints the number of successful operations and
  * exits the program
  */
void sigfpe_handler() {
	printf("\nError: a division by 0 operation was attempted.\n");
	printf("Total number of operations completed successfully: %i\n", count);
	printf("The program will be terminated\n");
	exit(0);
}

/**
  * Handles the SIGINT interrupt, prints the number of successful operations and
  * exits the program
  */
void sigint_handler() {
	printf("\nThe total number of operations completed successfully: %i\n", count);
	printf("The program will be terminated\n");
	exit(0);
}

/**
  * Main method of the program, performs the division and sets up the handlers
  */
int main() {

	count = 0;

	//FPE handler
	struct sigaction sa_sigfpe;
	memset(&sa_sigfpe, 0, sizeof(sa_sigfpe));
	sa_sigfpe.sa_handler = sigfpe_handler;
	if(sigaction(SIGFPE, &sa_sigfpe, NULL) != 0) {
		printf("Error in SIGFPE\n");
		exit(0);
	}

	//INT handler
	struct sigaction sa_sigint;
	memset(&sa_sigint, 0, sizeof(sa_sigint));
	sa_sigint.sa_handler = sigint_handler;
	if(sigaction(SIGINT, &sa_sigint, NULL) != 0) {
		printf("Error in SIGINT\n");
		exit(0);
	}

	while(1) {

		int num, dem;
		char nums[100];

		printf("Enter the first integer: ");
		fgets(nums, 100, stdin);
		num = atoi(nums);

		printf("Enter the second integer: ");
		fgets(nums, 100, stdin);
		dem = atoi(nums);

		int ans = num / dem;
		int rem = num % dem;

		printf("%i / %i is %i with a remainder of %i\n", num, dem, ans, rem);
		count++;

	}

	return 0;

}
