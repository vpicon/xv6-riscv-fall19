#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"




int main(int argc, char *argv[]) {
    // Variables for pingpong
    char msg = 0xfe;

    // Open child pipe and parent pipe
    int parent_fd[2], child_fd[2];
    pipe(parent_fd);
    pipe(child_fd);

    // Create child process
    int pid;
    if ((pid = fork()) < 0) {
        fprintf(2, "fork error\n");
        exit();
    }

    // PingPong byte
    if (pid == 0) {  // Child process
        char c;
        read(parent_fd[0], &c, 1);
        printf("%d: received ping\n", getpid());

        write(child_fd[1], &msg, 1);
        exit();
    }

    // Parent process
    write(parent_fd[1], &msg, 1);

    char c;
    read(child_fd[0], &c, 1);
    printf("%d: received pong\n", getpid());

    // Wait for child and exit
    wait();
    exit();
}

