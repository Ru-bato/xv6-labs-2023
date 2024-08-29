#include "user/user.h"

int
main(int argc, char *argv[])
{
    if(argc != 1)
        exit(1); // for safety
    // create 2 pfd as pipe, one is from parent to child, the other is opposite
    // index 0 is read end, 1 is write end;
    int pfd1[2]; // parent to child
    int pfd2[2]; // child to parent
    char buf[1]; // 1 byte info

    pipe(pfd1);
    pipe(pfd2);

    if(fork() == 0){ // child processor
        // close parent's write and child's read
        close(pfd1[1]);
        close(pfd2[0]);

        read(pfd1[0], buf, 1); // read 1 byte from parent
        fprintf(1, "%d: received ping\n", getpid());

        write(pfd2[1], buf, 1); // write 1 byte to parent
        close(pfd1[0]);
        close(pfd2[1]);
    }
    else{
        // close parent's read and child's write
        close(pfd1[0]);
        close(pfd2[1]);

        write(pfd1[1], "A", 1);
        read(pfd2[0], buf, 1);
        fprintf(1, "%d: received pong\n", getpid());

        close(pfd1[1]);
        close(pfd2[0]);

        // wait child finish
        wait(0);
    }
    exit(0);
}