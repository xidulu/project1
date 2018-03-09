#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <wordexp.h>
#define MAXLEN 100
#define CMDMAX 100


char** lineSplit(char* line) {
    // FIXME
    return NULL;
}

int changeInput(char* file) {

}

int changeOutput(char* file) {

}

int changeDirectory(char** args) {
    if (args[1] == NULL) {
        chdir(getenv("HOME"));
        return 1;
    }
    if (chdir(args[1]) == -1) {
        printf("Directory not found");
        return -1;
    }
    return 0;
}

/**
 * args:argument list
 * background: 1 for running background, 0 otherwise
 * redirect= 0:output to file. 1:concatenate to file. 2:for input from file
 * FILE: string for filename
 * */
int launchProgram(char** args, int background,
                    int redirect, char* file) {
    int status;
    pid_t pid = fork();
    if (pid == 0)
    {
        if (redirect == 0) freopen(file, "w", stdout);
        if (redirect == 1) freopen(file, "at", stdout);
        if (redirect == 2) freopen(file, "rt", stdin);
        if(execvp(args[0], args) < 0) {
            printf("execute error!\n");
        }
        return 0;
    }
    else if (!background) {
        wait(NULL);
    } 
    return 0;
}  

int printCurrentDirectory() {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL)
        fprintf(stdout, "Current working dir: %s\n", cwd);
    else
        perror("getcwd() error");
    return 0;
}

/**
 * Unit test for cd command (kind of)
 * */
int testCdCommand() {
    printCurrentDirectory();
    char* args[2];
    strcpy(args[0], "cd");
    strcpy(args[1], "..");
    changeDirectory(args);
    printCurrentDirectory();
}

int main() {
    while(1) {
        printf("shell#");
        char buffer[MAXLEN];
        fgets(buffer, MAXLEN, stdin);
        buffer[strcspn(buffer, "\n")] = 0;
        char *args[CMDMAX + 1] = {0};
        int n_cmds = 0;
        int background = 0;
        int redirect = -1;
        char *nextToken;
        char* file;
        args[0] = strtok(buffer," ");
        while ((nextToken = strtok(NULL, " ")) != NULL)
        {
            if (redirect != -1) {
                n_cmds++;
                file = nextToken;
                continue;
            }
            if (strcmp(nextToken, "&") == 0) {
                background = 1;
                continue;
            }
            if (strcmp(nextToken, ">") == 0) {
                n_cmds++;
                redirect = 0;
                continue;
            }
            if (strcmp(nextToken, ">>") == 0) {
                n_cmds++;
                redirect = 1;
                continue;
            }
            if (strcmp(nextToken, "<") == 0) {
                n_cmds++;
                redirect = 2;
                continue;
            }
            n_cmds++;
            args[n_cmds] = nextToken;
        }
        args[n_cmds + 1] = NULL;
        if (strcmp(args[0], "cd") == 0) {
            int t = changeDirectory(args);
            continue;
        }
        if (strcmp(args[0], "exit") == 0) {
            break;
        }
        launchProgram(args, background,
                redirect, file);
    }
    return 0;
}
 
