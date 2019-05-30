////////////////////////////////////////////////////////////////////////////////
// Main File: intdate.c
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

#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

int globaltime = 3; //alarm time
int count = 0; //SIGUSR1 count

/**
  * Handles the SIGALRM interrupt and prints the time and date every 3 seconds
  */
void sigalrm_handler() {
	time_t timenow;
	time(&timenow);

	printf("PID: %d", getpid());
	printf(" | Current Time: %s", ctime(&timenow));

	if(alarm(globaltime) != 0) {
		printf("Error when setting up alarm\n");
		exit(0);
	}
}

/**
  * Handles the SIGINT interrupt, prints how many times SIGUSR1 was received, and
  * exits the program
*/
void sigint_handler() {
	printf("SIGINT received.\n");
	printf("SIGUSR1 was received %i times. Exiting now.\n", count);
	exit(0);
}

/**
  * Handles the SIGUSR1 interrupts, prints that it was caught and increments
  * the counter
  */
void sigusr1_handler() {
	printf("SIGUSR1 caught!\n");
	count++;
}

/**
  * Main function of the program, sets up the handlers and runs in an infinite loop
  */
int main(int argc, char *argv[]) {

	printf("Date will be printed every 3 seconds.\n");
	printf("Enter ^C to end the program.\n");

	if(alarm(3) != 0) {
		printf("Error setting up the alarm.\n");
	}

	//ALRM handler
	struct sigaction sa_alrm;
	memset(&sa_alrm, 0, sizeof(sa_alrm));
	sa_alrm.sa_handler = sigalrm_handler;
	if(sigaction(SIGALRM, &sa_alrm, NULL) != 0) {
		printf("Error in sigaction() associated with SIGALRM.");
		exit(0);
	}

	//USR1 handler
	struct sigaction sa_usr;
	memset(&sa_usr, 0, sizeof(sa_usr));
	sa_usr.sa_handler = sigusr1_handler;
	if(sigaction(SIGUSR1, &sa_usr, NULL) != 0) {
		printf("Error in sigaction() associated with SIGUSR1.\n");
		exit(0);
	}

	//INT handler
	struct sigaction sa_int;
	memset(&sa_int, 0, sizeof(sa_int));
	sa_int.sa_handler = sigint_handler;
	if(sigaction(SIGINT, &sa_int, NULL) != 0) {
		printf("Error in sigaction() associated with SIGINT.\n");
		exit(0);
	}

	while(1) {
	}

	return 0;
}
