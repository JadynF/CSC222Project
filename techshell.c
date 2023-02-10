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

// return the number of arguments in the input
int inputLen(char input[]) {
	// copy of input
	char inputcpy[255];
	strcpy(inputcpy, input);
	char* tok = strtok(inputcpy, " ");
	int count = 0;
	// loop through input, count number of args
	while (tok != NULL) {
		tok = strtok(NULL, " ");
		count++;
	}
	return count;
}

// parse the input by space and add arguments to list
void parseInput(char input[], char *args[]) {
	char* tok = strtok(input, " ");
	int i = 0;
	while (tok != NULL) {
		args[i] = (char *) malloc((strlen(tok) + 1) * sizeof(char));
		strcpy(args[i++], tok);
		tok = strtok(NULL, " ");
	}
	// get rid of last 2 characters in last element (\n)
	i--;
	int len = strlen(args[i]);
	args[i][len] = '\0';
	args[i][len - 1] = '\0';
	args[i + 1] = NULL;
}

// execute the input
void execCommand(char *args[], char *infile[], char *outfile[]) {
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

// get the names of the redirection files
void getReDirFiles(char* args[], char *infile[], char *outfile[]) {
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
	char* infile[1];
	char* outfile[1];
	printDir();
	// loop to get input
	while (fgets(input, 255, stdin) !=  NULL) {
		// exit case
		if (!strcmp(input, "exit\n"))
			break;
		// set size of argument list
		char* args[inputLen(input) + 1];
		// parsed arguments by space and add to argument list
		parseInput(input, args);
		// get the names of redirection files
		getReDirFiles(args, infile, outfile);
		// execute arguments
		execCommand(args, infile, outfile);
		// print curr directory
		printDir();
		// reset file names
		infile[0] = NULL;
		outfile[0] = NULL;
	}
	return 0;
}
