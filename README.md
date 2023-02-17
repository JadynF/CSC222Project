# CSC222Project
A simple shell made in c

Names: Jadyn Falls, Lael Hamilton

Allows a user to repeatedly enter input to run commands through the main function. Once recieved, the users input is parsed, and an argument array is dynamically built with malloc and realloc and redirection files are gotten in the parseInput function. Then the command is executed in the execCommand function by forking the process and having the child do an execvp call with the arguments provided from the parse input function.
