#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <wordexp.h>
#include <fcntl.h>

#define MAXLEN 100
#define CMDMAX 100
#define HISTORYMAX 100

char history[MAXLEN][HISTORYMAX];
int recordNum = 0;

int background = 0;

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

// for debug usage
void printArgs(char** args) {
    int  i;
    for(i = 0; i < 5; i++) {
        printf("%s ", args[i]);
    }
    printf("\n");
}

int executeProcess(char** args, int in, int out) {
    if (strcmp(args[0], "cd") == 0) {
        return changeDirectory(args);
    }
    // printArgs(args);
    pid_t pid;
    if ((pid = fork()) == 0)
    {
        if (in != 0) {
            dup2(in, 0);
            close(in);
        }
        if (out != 1) {
            dup2(out, 1);
            close(out);
        }
        if (execvp(args[0], args) < 0) {
            printf("Execution error!\n");
            exit(-1);
        }
        return 0;
    } else if (!background){
        wait(NULL);
    }
    return pid;
}


int printCurrentDirectory() {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL)
        fprintf(stdout, "%s", cwd);
    else
        perror("getcwd() error");
    return 0;
}


int str2num(char* str) {
    int len = strlen(str);
    int result = 0;
    int i;
    for(i = 0; i < len; i++) {
        result = result * 10 + str[i] - '0';
    }
    return result;
}

/**
 * Parse a line.
 * Read tokens one by one, from left to right.
 **/
int parseLine(char* buffer) {
    char *args[CMDMAX + 1] = {0};
    int n_cmds = 0;
    int redirect = -1;
    int in = 0;
    int out = 1;
    
    char *nextToken;
    char *file;
    args[0] = strtok(buffer," ");
    if (args[0] == NULL) {
        return 0;
    }
    if (strcmp(args[0], "exit") == 0) {
        exit(0);
    }
    if (strcmp(args[0], "history") == 0) {
        char* numStr;
        if ((numStr  = strtok(NULL, " ")) == NULL) {
            int i;
            for(i = 0; i < recordNum - 1; i++) {
                printf("%s", history[i]);
            }
        } else {
            int numVal = str2num(numStr);
            int i;
            if (numVal > recordNum - 1) {
                printf("Out of range!\n");
                return -1;
            }
            for(i = recordNum - 1 - numVal;
                i < recordNum - 1; i++) {
                    printf("%s", history[i]);
                }
        }
        return 0;
    }
    while(1) {
        // end of a line.
        if ((nextToken = strtok(NULL, " ")) == NULL) {
            // no token before EOL
            if (n_cmds == -1) {
                break;
            }
            args[++n_cmds] = NULL;
            executeProcess(args, in, out);
            break;
        }
        if (strcmp(nextToken, "&") == 0) {
            background = 1;
            continue;
        }
        if (strcmp(nextToken, "|") == 0) {
            int fd[2];
            pipe(fd);
            args[++n_cmds] = NULL;
            if (executeProcess(args, in, fd[1]) < 0) {
                break;
            }
            close(fd[1]);
            // input source for next process.
            in = fd[0];
            n_cmds = -1;
            background = 0;
            continue;
        }
        if (strcmp(nextToken, ">") == 0) {
            args[++n_cmds] = NULL;
            file = strtok(NULL, " ");
            int fd = open(file, O_RDWR | O_CREAT);
            if (executeProcess(args, in, fd) < 0) {
                break;
            }
            close(fd);
            n_cmds = -1;
            background = 0;
            continue;
        }
        if (strcmp(nextToken, ">>") == 0) {
            args[++n_cmds] = NULL;
            file = strtok(NULL, " ");
            int fd = open(file, O_RDWR | O_CREAT | O_APPEND);
            if (executeProcess(args, in, fd) < 0) {
                break;
            }
            close(fd);
            n_cmds = -1;
            continue;
        }
        if (strcmp(nextToken, "<") == 0) {
            args[++n_cmds] = NULL;
            file = strtok(NULL, " ");
            int fd = open(file, O_RDWR);
            if (executeProcess(args, fd, out) < 0) {
                break;
            }
            close(fd);
            n_cmds = -1;
            continue;
        }
        args[++n_cmds] = nextToken;
    }
    return 0;
}


int main() {
    while(1) {
        fflush(stdout);
        printCurrentDirectory();
        printf(":");
        char buffer[MAXLEN];
        fgets(buffer, MAXLEN, stdin);
        strcpy(history[recordNum++], buffer);
        buffer[strcspn(buffer, "\n")] = 0;
        parseLine(buffer);
    }
    return 0;
}
 
