#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Function protoptypes
void handleHelp(int, char *argv[]);
void startShell();
void helpMessage();
char* removePreWhiteSpace(char*, ssize_t);
char* getCommand(char*);

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
				helpMessage();
				exit(0);
				break;
			// Any illegal actions causes error message and exit
			case '?':
				exit(EXIT_FAILURE);
		}
	}

}

void helpMessage()
{
	// TODO: implement the help message
	printf("THIS IS A HELP MESSAGE\n");
}

void startShell()
{
	FILE *fs = stdin;			// default filestream is stdin
	char *line = NULL;			// line to be read from stdin
	size_t len = 0;				// length of line to be read cap
	ssize_t lineLen;			// length of line that was read
	char *line_wsPre_rm;		// line without the whitespace in beginning
	char *command;				// The command(w/o any args if any are provided)

	while (1)
	{
		printf("> ");			// prompting

		// getline takes line, len and fs as parameters
		// line and len are dynamically allocated if there is no space avail
		// or if line is NULL and len is 0
		// fs is pointed at stdin, so we will get a line from stdin
		// -1 returned if theres an error, so we print out error
		if ((lineLen = getline(&line, &len, fs)) == -1)
		{
			fprintf(stderr, "Unable to read from stdin.\n");
		}
		
		// Handle whitespace before going through 
		line_wsPre_rm = removePreWhiteSpace(line, lineLen);
		command = getCommand(line_wsPre_rm);

		printf("command is:%s\n", command);

		// TODO: maybe free (line) here?




	}
}

char* removePreWhiteSpace(char *line, ssize_t lineLength)
{
	int wsCount = 0;			// to look for first instance of character
	char *ret_line = NULL;		// line to return with no pre-whitespace
	int newLineLen = 0;			// just to keep track of ret_line's new length


	// Look for the first instance of char that isn't whitespace
	for (int i = 0; i < lineLength; i++)
	{
		// TODO: maybe account for \t or other whitespace chars?
		if (line[i] != ' ') 
		{
			wsCount = i;
			break;
		}
	}	
	
	// newLineLen is the new line's length, used for malloc and moving chars
	// over to the ret_line
	newLineLen = lineLength-wsCount;

	// malloc new space
	if (!(ret_line = malloc(sizeof(char) * newLineLen)))
	{
		fprintf(stderr,"insufficient memory");
		exit(EXIT_FAILURE);
	}
	
	// Copy over line's content (so that no whitespace in beginning)
	for (int j = 0; j < newLineLen; j++)
	{
		ret_line[j] = line[wsCount];
		wsCount++;
	}

	return ret_line; 
}

// getCommand gets the command given a line
// More generally, it gets the word before it hits a whitespace
char* getCommand(char *line)
{
	char* ret = NULL;
	size_t retLen = 0; 
	size_t i = 0;

	// Look for first whitespace
	while(line[i] != ' ' && line[i] != '\t')
	{
		retLen++;
		i++;
	}

	// malloc new space for ret.
	// catch error if there is
	if (!(ret = malloc(sizeof(char) * retLen)))
	{
		fprintf(stderr,"insufficient memory");
		exit(EXIT_FAILURE);
	}

	// Copy over the command to ret_line
	i = 0;
	for (int j = 0; j < retLen; j++)
	{
		ret[j] = line[i];
		i++;
	}

	return ret;
}
