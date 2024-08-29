#include "kernel/types.h"
#include "user/user.h"
#include "kernel/param.h"

// read a line from stdin
int 
readline(char *buf, int max) {
    int i = 0;
    while (i < max - 1) {
        if (read(0, &buf[i], 1) <= 0) {
            break; // EOF or error
        }
        if (buf[i] == '\n') {
            buf[i] = '\0';
            return i;
        }
        i++;
    }
    buf[i] = '\0';
    return i;
}

int 
main(int argc, char *argv[]) {
    char buf[512];
    char *new_argv[MAXARG];
    int n;

    // init new_argv
    for (int i = 0; i < argc - 1; i++) {
        new_argv[i] = argv[i + 1];
    }

    while ((n = readline(buf, sizeof(buf))) > 0) {
        if (fork() == 0) {
            // child processor
            new_argv[argc - 1] = buf; // make the input line to be the additional parameter
            new_argv[argc] = 0;       // the last element of argv must be NULL
            exec(new_argv[0], new_argv); // execute command
            // execute failure
            fprintf(2, "exec %s failed\n", new_argv[0]);
            exit(1);
        } else {
            // parent processor
            wait(0); // wait child
        }
    }

    exit(0);
}
