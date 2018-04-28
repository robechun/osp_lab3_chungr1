#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Function protoptypes
void handleHelp(int, char *argv[]);
void startShell();


int main (int argc, char *argv[])
{

	handleHelp(argc, argv);
	startShell();



	
	return 0;
}

// handleHelp uses getopt to parse command line options
// if -h is used, prints out a help message and exits.
// Only happens during falsh invocation
void handleHelp(int argc, char *argv[])
{
	int opt;		// variable needed for getopt

	// getopt gets the next option argument from argc and argv
	// "h" is to indicate that -h is a valid argument
	// returns -1 when nothing else is to be read
	while ((opt = getopt(argc, argv, "h")) != -1)
	{
		switch(opt)
		{
			// Help message if correctly invoked
			case 'h':
				printf("THIS IS A HELP MESSAGE\n");
				exit(0);
				break;
			// Any illegal actions causes error message and exit
			case '?':
				exit(EXIT_FAILURE);
		}
	}

}

void startShell()
{
	while (1)
	{

	}
}
