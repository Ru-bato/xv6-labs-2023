#include "user/user.h"

void 
prime_sieve(int left_pipe_read) 
{
    int p[2];
    int prime;
    
    // Read the first number, which is a prime
    if (read(left_pipe_read, &prime, sizeof(prime)) == 0) {
        // read returns zero when the write-side of a pipe is closed
        close(left_pipe_read);
        exit(0);
    }

    fprintf(1, "prime %d\n", prime);

    // Set up the next pipe
    pipe(p);

    if (fork() == 0) {
        // Child process to handle the next prime
        close(p[1]);          // Close write end
        close(left_pipe_read); // Close the read
        prime_sieve(p[0]);    // Recur for the next prime
    } else {
        // Parent process filters numbers
        int num;
        close(p[0]); // Close read end

        while (read(left_pipe_read, &num, sizeof(num)) != 0) {
            if (num % prime != 0) {
                write(p[1], &num, sizeof(num));  // Pass non-multiples down the pipeline
            }
        }
        
        // Cleanup
        close(left_pipe_read);
        close(p[1]); // Close write end
        wait(0); // Wait for the child process to finish
        exit(0);
    }
}

int 
main(int argc, char *argv[]) 
{
    int p[2]; // parent to child, single direction
    pipe(p);

    if (fork() == 0) {
        // Child process handles the prime sieve
        close(p[1]);  // Close write end
        prime_sieve(p[0]);
    } else {
        // Parent process feeds numbers into the pipeline
        close(p[0]);  // Close read end
        for (int i = 2; i <= 35; i++) {
            write(p[1], &i, sizeof(i));  // Write numbers 2 to 35
        }
        close(p[1]);  // Close write end to signal end of input
        wait(0);      // Wait for the child process to finish
    }
    exit(0);
}
