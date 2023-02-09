#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

// print out current working directory
void printDir() {
	char dir[100];
	printf("%s$ ", getcwd(dir, 100));
}

// return the number of arguments in the input
int inputLen(char input[], int* reDirs) {
	// copy of input
	char inputcpy[255];
	strcpy(inputcpy, input);
	char* tok = strtok(inputcpy, " ");
	int count = 0;
	// loop through input, count number of args
	while (tok != NULL) {
		// if token is a redirection, increment redir globally
		if (!strcmp(tok, ">") || !strcmp(tok, "<")) {
			*reDirs = *reDirs + 1;
		}
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
void execCommand(char *args[]) {
		// check if a chdir call
		if (args[0] != NULL && strcmp(args[0], "cd")) {
			pid_t pID = fork();
			// child executes command
			if (pID == 0) {
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

void reDirIn(char* args[], char file[]) {

}

void reDirOut(char* args[], char file[]) {

}

// main
int main(int argc, char argv[]) {
	char input[255];
	printDir();
	// loop to get input
	while (fgets(input, 255, stdin) !=  NULL) {
		// exit case
		if (!strcmp(input, "exit\n"))
			break;
		// count number of redirections
		int reDirs = 0;
		// set size of argument list
		char* args[inputLen(input, &reDirs) + 1];
		// parsed arguments by space and add to argument list
		parseInput(input, args);
		// execute arguments
		execCommand(args);
		// print curr directory
		printDir();
	}
	return 0;
}
