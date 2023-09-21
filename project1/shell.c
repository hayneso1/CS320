#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

// Constants
#define MAX_BUFFER 1024
#define MAX_TOKENS 64

// function to take an input and parse tokens using strtok library function
void tokenize(char *input, char **tokens)
{
    char *token;
    int count = 0;

    token = strtok(input, " \t\n");

    // loop through input add to tockens
    while (token != NULL && count < MAX_TOKENS)
    {
        tokens[count] = token;
        token = strtok(NULL, " \t\n");
        count++;
    }
	tokens[count] = NULL;
}

int main()
{
    char input[MAX_BUFFER];
    char *tokens[MAX_TOKENS];

    // loop until end-of-file == '\0'
    while (1)
    {
        printf("shell>");

        // stdin == end-of-file, quit program
        if (fgets(input, MAX_BUFFER, stdin) == NULL)
        {
            break;
        }

        // Remove the newline character
        input[strlen(input) - 1] = '\0';

        if (input[0] == '\0')
        {
            // exit on empty line
            break;
        }

        // Tokenize input
        tokenize(input, tokens);

        // Fork a child process
        int pid;
        pid = fork();

        if (pid == 0)
        {
            // I am a child
            // printf("I am the child.")
            if (execvp(tokens[0], tokens) == -1)
            {
                perror("Command execution error");
                exit(1);
            }
        }
        else
        {
            // I am the parent
            // printf("I am the parent.")
            int status;
            waitpid(pid, &status, 0);
        }
    }

    return 0;
}
