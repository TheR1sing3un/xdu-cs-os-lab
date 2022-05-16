#include<fcntl.h>
#include<sys/types.h>
#include<stdio.h>
#include<unistd.h>
#include<sys/stat.h>

// the pipe's location
#define FIFO_SERVER "/tmp/fifoserver"

int main() {
    // create fifo
    if (mkfifo(FIFO_SERVER, O_NONBLOCK) == -1) {
        // create fifo error
        printf("[%d] create fifo error\n", getpid());
        return -1;
    }
    // create fifo success
    printf("[%d] create fifo success\n", getpid());
    // fork a child process
    pid_t p = fork();
    if (p == -1) {
        // fork error
        printf("[%d] fork a child process error\n", getpid());
        return -1;
    }
    if (p) {
        // parent process
        // open the fifo with readonly mode
        int fd = open(FIFO_SERVER, O_RDONLY);
        char buf[255];
        // read data from fifo
        read(fd, buf, 255);
        printf("[%d] read fifo success. Data: %s\n", getpid(), buf);
        // close fd
        close(fd);
    } else {
        // child process
        // open the fifo with writeonly mode
        int fd = open(FIFO_SERVER, O_WRONLY);
        char buf[255] = "Hello, my deer friend!\n";
        write(fd, buf, 255);
        printf("[%d] write fifo success. Data: %s\n", getpid(), buf);
        // close fd
        close(fd);
    }
}