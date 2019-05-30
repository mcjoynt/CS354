////////////////////////////////////////////////////////////////////////////////
// Main File: sendsig.c
// This File: intdate.c
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
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <string.h>

/**
  * Main method of the program, takes argument inputs and sends signals to
  * other processes
  */

int main(int argc, char *argv[]) {

	if (argc != 3) {
		printf("Usage: <signal type> <pid>\n");
		exit(0);
	}

	pid_t pid = atoi(argv[2]);

	int uString = strcmp("-u", argv[1]);

	int uInt = strcmp("-i", argv[1]);

	if (uString == 0) {
		kill(pid, SIGUSR1);
	}

	if(uInt == 0) {
		kill(pid, SIGINT);
	}

}
