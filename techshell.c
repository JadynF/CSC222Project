///////////////////////////////////////////
// Names: Jadyn Falls, Lael Hamilton
// Date: 2/11/23
// Desc: Simple shell with I/O redirections
// and dynamicly allocated arguments
///////////////////////////////////////////
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>

// function prototypes
void printDir();
char** parseInput(char input[], char* infile[], char* outfile[]);
void execCommand(char** args, char* infile[], char* outfile[]);

// main that loops for input and calls other functions
int main(int argc, char argv[]) {
	// strings for input, args, and redirection file names
	char input[255];
	char* infile[1];
	char* outfile[1];
	char** args = NULL;
	printDir();
	// loop to get input
	while (fgets(input, 255, stdin) !=  NULL) {
		// set strings to NULL
		infile[0] = NULL;
		outfile[0] = NULL;
		args = NULL;
		// exit case
		if (!strcmp(input, "exit\n"))
			break;
		// get argument list and redirection files
		args = parseInput(input, infile, outfile);
		// execute arguments
		execCommand(args, infile, outfile);
		// print curr directory
		printDir();
	}
	return 0;
}

// print out current working directory
void printDir() {
	char dir[100];
	printf("%s$ ", getcwd(dir, 100));
}

// parse the input, add arguments to list, and get redirection file names
char** parseInput(char input[], char* infile[], char* outfile[]) {
	// initial array of arguments
	char** args = (char **) malloc(sizeof(char*));
	// delimiters for tokens
	char delim[] = " \n";
	char* tok = strtok(input, delim);
	int i = 0;
	while (tok != NULL) {
		// check for file redirections
		if (!strcmp(tok, "<")) {
			// move to next token
			tok = strtok(NULL, delim);
			// allocate space and save file name
			infile[0] = (char *) malloc(sizeof(char*) * strlen(tok));
			strcpy(infile[0], tok);
		}
		else if (!strcmp(tok, ">")) {
			// move to next token
			tok = strtok(NULL, delim);
			// allocate space and save file name
			outfile[0] = (char *) malloc(sizeof(char*) * strlen(tok));
			strcpy(outfile[0], tok);
		}
		else {
			// allocate for new element in argument list
			args = (char **) realloc(args, sizeof(char*) * (i + 2));
			// set element to token
			args[i] = tok;
			i++;
		}
		tok = strtok(NULL, delim);
	}
	// set end of args arr to null
	args[i] = NULL;
	return args;
}

// execute the input
void execCommand(char** args, char* infile[], char* outfile[]) {
	// check if a chdir call
	if (args[0] != NULL && strcmp(args[0], "cd")) {
		pid_t pID = fork();
		// child executes command
		if (pID == 0) {
			// if there is a file name for redirection
			if (infile[0] != NULL) {
				FILE* openedInfile = fopen(infile[0], "r");
				// if file exists and for other errors
				if (openedInfile == NULL) {
					printf("Error %d (%s)\n", errno, strerror(errno));
					exit(0);
				}
				dup2(fileno(openedInfile), 0);
				fclose(openedInfile);
			}
			if (outfile[0] != NULL) {
				FILE* openedOutfile = fopen(outfile[0], "w");
				// check for error when opening file
				if (openedOutfile == NULL) {
					printf("Error %d (%s)\n", errno, strerror(errno));
					exit(0);
				}
				dup2(fileno(openedOutfile), 1);
				fclose(openedOutfile);
			}
			// execute command
			if ((execvp(args[0], args)) == -1)
				printf("Error %d (%s)\n", errno, strerror(errno));
			exit(0);
		}
		wait(0);
	}
	// if changing dir
	else {
		if (chdir(args[1]) != 0)
			printf("Error %d (%s)\n", errno, strerror(errno));
	}
}

