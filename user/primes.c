#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"


void primes(int *parent_fd) {
    // Close given write end
    close(parent_fd[1]);

    // read first received prime (if any) and print it to stdout, or finish
    int p;
    if (read(parent_fd[0], &p, sizeof(p)) == 0) { // trivial end of recursion
        close(parent_fd[0]);
        exit(0);
    }
    fprintf(1, "prime %d\n", p);

    // Open a pipe and create new child to send numbers to
    int child_fd[2];
    if (pipe(child_fd) < 0) {
        fprintf(2, "pipe error\n");
        exit(1);
    }

    int pid;
    if ((pid = fork()) < 0) {
        fprintf(2, "fork error\n");
        /* exit(1); */
    }
    // child side
    if (pid == 0) {
        close(parent_fd[0]);
        primes(child_fd);
    }

    close(child_fd[0]); // close read end of child pipe

    // read from parent all other received numbers and send to child
    // those numbers which p does not divide
    while (1) {
        int n;
        if (read(parent_fd[0], &n, sizeof(int)) == 0) // EOF received
            break;

        if (n % p != 0)  // p does not divide n
            write(child_fd[1], &n, sizeof(int));
    }

    // close used pipe ends
    close(parent_fd[0]);
    close(child_fd[1]);

    // wait for child to finish and exit
    wait(0);
    exit(0);
}



int main(int argc, char *argv[]) {
    // Open child pipe and parent pipe
    int fd[2];
    if (pipe(fd) < 0) {
        fprintf(2, "pipe error\n");
        exit(1);
    }

    // Create child process
    int pid;
    if ((pid = fork()) < 0) {
        fprintf(2, "fork error\n");
        exit(1);
    }

    // Child process
    if (pid == 0)
        primes(fd);

    // Parent process
    close(fd[0]);  // close read end
    for (int i = 2; i <= 35; i++)
        write(fd[1], &i, sizeof(int));

    close(fd[1]);  // close write end (send EOF)


    // Wait for child and exit
    wait(0);
    exit(0);
}

