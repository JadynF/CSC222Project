///////////////////////////////////////////
// Names: Jadyn Falls, Lael Hamilton
// Date: 2/11/23
// Desc: Simple shell with I/O redirections
///////////////////////////////////////////
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>

// print out current working directory
void printDir() {
	char dir[100];
	printf("%s$ ", getcwd(dir, 100));
}

// parse the input by space and add arguments to list
char** parseInput(char input[]) {
	char** args = (char **) malloc(sizeof(char*));
	char* tok = strtok(input, " ");
	int i = 0;
	while (tok != NULL) {
		// allocate for new element in argument list
		args = (char **) realloc(args, sizeof(char*) * i + 2);
		// allocate space in index for element
		args[i] = tok;
		tok = strtok(NULL, " ");
		i++;
	}
	// get rid of last 2 characters in last element (\n)
	int len = strlen(args[i - 1]);
	args[i - 1][len] = '\0';
	args[i - 1][len - 1] = '\0';
	args[i] = NULL;
	return args;
}

// execute the input
void execCommand(char** args, char *infile[], char *outfile[]) {
		printf("1: %s.\n", args[0]);
		printf("2: %s.\n", args[1]);
		printf("3: %s.\n", args[2]);
		// check if a chdir call
		if (args[0] != NULL && strcmp(args[0], "cd")) {
			printf("child!");
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

// get the names of the redirection files
void getReDirFiles(char** args, char *infile[], char *outfile[]) {
	printf("1: %s.\n", args[0]);
	printf("2: %s.\n", args[1]);
	printf("3: %s.\n", args[2]);
	int i = 0;
	while (args[i] != NULL) {
		// if redirecting into file
		if (!strcmp(args[i], ">")) {
			// set outfile to file name
			outfile[0] = (char *) malloc(sizeof(char) * strlen(args[i + 1]));
			strcpy(outfile[0], args[i + 1]);
			// remove redirection from arguments
			args[i] = NULL;
			args[i + 1] = NULL;
			i++;
		}
		// if input redirected from file
		else if (!strcmp(args[i], "<")) {
			// set infile to file name
			infile[0] = (char*) malloc(sizeof(char) * strlen(args[i + 1]));
			strcpy(infile[0], args[i + 1]);
			// remove redirection from arguments
			args[i] = NULL;
			args[i + 1] = NULL;
			i++;
		}
		i++;
	}
}

// main
int main(int argc, char argv[]) {
	// strings for input and redirection file names
	char input[255];
	printDir();
	// loop to get input
	while (fgets(input, 255, stdin) !=  NULL) {
		// exit case
		if (!strcmp(input, "exit\n"))
			break;
		// set argument list
		char** args = parseInput(input);
		// parsed arguments by space and add to argument list
		printf("1: %s.\n", args[0]);
		printf("2: %s.\n", args[1]);
		printf("3: %s.\n", args[2]);
		// get the names of redirection files
		char* infile[1];
		char* outfile[1];
		getReDirFiles(args, infile, outfile);
		printf("1: %s.\n", args[0]);
		printf("2: %s.\n", args[1]);
		printf("3: %s.\n", args[2]);
		// execute arguments
		execCommand(args, infile, outfile);
		// print curr directory
		printDir();
		// reset file names
		infile[0] = NULL;
		outfile[0] = NULL;
		args = NULL;
	}
	return 0;
}
