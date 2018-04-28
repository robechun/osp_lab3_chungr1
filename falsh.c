#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Function protoptypes
void handleHelp();


int main (int argc, char *argv[])
{
	int opt;

	while ((opt = getopt(argc, argv, "h")) != -1)
	{
		switch(opt)
		{
			case 'h':
				handleHelp();
				break;
			default:
				fprintf(stderr, "Unknown arugment");
		}
	}
	return 0;
}

// handleHelp prints out a help message and exits.
// Only happens during falsh invocation
void handleHelp()
{
	printf("THIS IS A HELP MESSAGE\n");
	exit(EXIT_FAILURE);
}
