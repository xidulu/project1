#include <stdio.h>
#include <syslib.h>

int main() {
    char *args[2];
    args[0] = "/bin/ls";
    args[1] = NULL;
    execv(args[0], args);
    return 0;
}
 
