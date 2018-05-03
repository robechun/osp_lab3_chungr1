#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>

// Function protoptypes
void handleHelp(int, char *argv[]);
void startShell();
void helpMessage();
char* removePreWhiteSpace(char*, ssize_t);
char* getCommand(char*);
void getPwd();
void handleCd(char*);
char* getArguments(char*);
void handleSetpath(char*);
void handleOtherCommands(char*, char*);
void handleRedirect(char*);

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

// helpMessage() prints out the help message
void helpMessage()
{
	// TODO: implement the help message
	printf("\"exit\" -- exits the shell\n\n\
\"pwd\" -- prints current working directory.\n\n\
\"cd [arg]\" -- changes directory to specified directory given\
in [arg]. If none specified, goes to $HOME dir\n\n\
\"setpath <dir> [dir] [dir] ... [dir]\" -- sets the path. User\
must provide at least one directory. Can set as manay\
as one wishes. Overwrites the path with given arguments\n\n\
\"help\" -- lists all the built in command with descriptions\n\n");
}


// startShell() starts the shell-- processes the commands sent by stdin
// Infinite loop until user exits out using "exit" or ctrl-c.
void startShell()
{
	FILE *fs = stdin;			// default filestream is stdin
	char *line = NULL;			// line to be read from stdin
	size_t len = 0;				// length of line to be read cap
	ssize_t lineLen;			// length of line that was read
	char *line_wsPre_rm;		// line without the whitespace in beginning
	char *command;				// The command(w/o any args if any are provided)
	char *arguments;			// arguments specified

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
		arguments = getArguments(line_wsPre_rm);
		
		// TODO: maybe free (line) here?
		
		if (strlen(arguments) > 0)
		{
			if (arguments[0] == '>')
			{
				handleRedirect(arguments);
			}
		}


		if (!strcmp(command, "exit"))
		{
			exit(0);
		}
		else if (!strcmp(command, "help"))
		{
			helpMessage();
		}
		else if (!strcmp(command, "pwd"))
		{
			getPwd();
		}
		else if (!strcmp(command, "cd"))
		{
			handleCd(arguments);
		}
		else if (!strcmp(command, "setpath"))
		{
			handleSetpath(arguments);
		}
		else
		{
			handleOtherCommands(command, arguments);
		}

	}
}

// removePreWhiteSpace removes all whitespace that might be present
//  in the beginning of the line.
// Returns the new line without the whitespace
char* removePreWhiteSpace(char *line, ssize_t lineLength)
{
	int wsCount = -1;			// to look for first instance of character
	char *ret_line = NULL;		// line to return with no pre-whitespace
	int newLineLen = 0;			// just to keep track of ret_line's new length


	// Look for the first instance of char that isn't whitespace
	for (int i = 0; i < lineLength; i++)
	{
		// TODO: maybe account for \t or other whitespace chars?
		if (line[i] != ' ' && line[i] != '\t' && line[i] != '\n') 
		{
			wsCount = i;
			break;
		}
	}	
	
	// if it only has whitespace, then need to account for it
	if (wsCount == -1)
	{
		wsCount = lineLength;
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
	while(line[i] != ' ' && line[i] != '\t' && line[i] != '\n')
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

// getPwd prints out the working directory
void getPwd()
{
	// cwd is where the pwd will be stored.
	char cwd[256];			

	// getcwd(arg1, arg2)
	// arg1 is where the results of the call will be stored
	// arg2 is to specify how much space is available in the cwd
	// returns null if there has been an error
	if (getcwd(cwd, sizeof(cwd)) == NULL)
	{
		fprintf(stderr, "getcwd failure!\n");
	}
	else
	{
		printf("%s\n",cwd);		// if successful, cwd should have working dir
	}
}	

// getArguments gets a line and grabs the argument(s) for the line
// removes leading whitespace
char* getArguments(char* line)
{
	char *ws_removed;
	int offset = 0;

	// go through to find first instance of a whitespace
	// This will tell you when the arguments start
	for (int i = 0; i < strlen(line); i++)
	{
		if (line[i] == ' ' || line[i] == '\t' || line[i] == '\n')
		{
			offset = i;
			break;
		}
	}

	// using the line, get a ws_removed line which has no leading whitespace
	// ws_removed will have the argument(s) without leading whitespace.
	ws_removed = removePreWhiteSpace(line+offset,strlen(line)-offset-1);

	return ws_removed;
}


// handleCd looks at the argument passed in and attempts to change directory
// if argument is empty, change directory to HOME env variable
void handleCd(char *arguments)
{
	char *homePath;

	// If the argument is empty, go to HOME dir
	if (strlen(arguments) == 0)
	{
		if (!(homePath = getenv("HOME")))
			fprintf(stderr, "Couldn't get HOME env variable\n");
		else
			chdir(homePath);	// chdir changes directory to path specified
	}
	else 
	{
		// chdir changes dir to path specified
		// retuns non-0 if unsuccessful.
		if (chdir(arguments) != 0)
			fprintf(stderr, "cd: no such file or directory: %s\n", arguments);
	}
		
}


// handleSetpath sets the $PATH env variable to the argument passed in
//  whitespace indicates another path
void handleSetpath(char *arguments)
{
	char *envVar = "PATH";				// env variable to set ($PATH)
	char allPaths[256] = {'\0'};		// the buffer to store all the paths
										// need \0 for strncat to work properly
	int prev = 0;						// keeps track of where to do strncat

	
	// Goes through the whole argument and parses out which parts to include
	//  for the path
	for (int i = 0; i < strlen(arguments); i++)
	{
		if (arguments[i] == ' ' || arguments[i] == '\t' || arguments[i] == '\n')
		{
			// add to allPaths one of the paths we found in the argument list
			strncat(allPaths, (arguments+prev),i-prev);
			strcat(allPaths, ":");

			// skip all other whitespace
			while (arguments[i] == ' ' || arguments[i] == '\t'
									   || arguments[i] == '\n')
			{
				i++;
			}

			// prev spot where concatenation will start from
			prev = i;

			i--;			// accounting for end-of-string error
		}
	}

	// Final push into allPaths.
	// This will work if theres only one path listed as well
	strcat(allPaths, arguments+prev);

	// setenv takes in 3 parameters
	// 1st param: the environment variable to set--PATH for us
	// 2nd param: the string that indicates what to set it as
	// 3rd param: non-zero indicates overload (which we want)
	if (setenv(envVar, allPaths, 1) == -1)
		fprintf(stderr, "error setting path");
	
	printf("$PATH=%s\n", getenv(envVar));
}
	
// handleOtherCommands takes in a command and arguments
// look into $PATH and execute command specified if you can.
void handleOtherCommands(char *command, char* args)
{
	char *token = args;					// token to be used for strtok
	char *argv[256] = { command, 0 };	// argv to be passed in for execvp
										// notice command already set to index 0

	int rc = fork();		// make a child process

	// Parse args passed in so that things like
	//  ls -al works
	// This sets up argv so that we can easily pass it in later to execvp
	int i = 1;
	while ((token = strtok(token, " ")) != NULL)
	{
		argv[i] = token;
		token = NULL;
	}
	
	if (rc < 0)
	{
		fprintf(stderr, "fork failed\n");
		exit(1);
	}
	else if (rc == 0)		// child process
	{
		// execvp takes in 2 arguments
		// looks in $PATH for commands
		// argv[0] is the command to execute
		// argv is the rest of the arguments (for when there's args to a
		// command)
		execvp(argv[0], argv);
	}
	else
	{
		wait(NULL);		// NULL says to wait for any child process to finish
	}
		
}

// TODO!!
void handleRedirect(char* args)
{
	printf("right here\n");
	char path[256] = { '.', '/', '\0'};

	strcat(path, (args+1));

	printf("2!!\n");
	int output =  open(strcat(path, ".out"), O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU);
	dup2(output, STDOUT_FILENO);

	int err = open(strcat(path, ".err"), O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU);
	dup2(err, STDERR_FILENO);

}
