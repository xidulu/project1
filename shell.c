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

int changeDirectory(char** args) {
    // printCurrentDirectory();
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

int launchProgram(char** args, int background) {
    int status;
    pid_t pid = fork();
    if (pid == 0)
    {
        if(execv(args[0], args) < 0) {
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
        printf("\nshell#");
        char buffer[MAXLEN];
        fgets(buffer, MAXLEN, stdin);
        buffer[strcspn(buffer, "\n")] = 0;
        // parse the line into argument list
        char *args[CMDMAX + 1] = {0};
        int n_cmds = 0;
        int background = 0;
        char *nextcmd;
        args[0] = strtok(buffer," ");
        while (args[n_cmds] != NULL)
        {
            n_cmds++;
            args[n_cmds] = strtok(NULL, " ");
        }
        args[n_cmds] = NULL;
        if (strcmp(args[0], "cd") == 0) {
            int t = changeDirectory(args);
        }

        if (strcmp(args[0], "exit") == 0) {
            break;
        }
        if (strcmp(args[n_cmds - 1], "&") == 0) {
            background = 1;
        }
        launchProgram(args, background);
    }
    return 0;
}
 
