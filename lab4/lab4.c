#include<wait.h>
#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<sys/types.h>

#define MAX_LINE 80;

int main() {
    int fd[2];
    if (pipe(fd) == -1 ){
        // create pipe failed
        printf("[%d] create pipe fail\n", getpid());
        return -1;
    }
    // create pipe successed
    // create a child process
    pid_t p = fork();
    if (p < 0) {
       // fork a process failed
       printf("[%d] fork a child process failed\n", getpid());
       return;
    }
    if (p) {
        // parent process
        // close read pipe
        close(fd[0]);
        char buf[255] = "hello,world";
        // wirte data
        write(fd[1], buf, 255);
        printf("[%d] write data success, data: %s\n", getpid(), buf);
        // close wirte pipe
        close(fd[1]);
        printf("[%d] close wirte-pipe success\n", getpid());
        // sleep a while
        sleep(1);
    } else{
        // child process
        // close write-pipe
        close(fd[1]);
        // sleep a while
        sleep(1);
        char buf[255];
        // read data from read-pipe
        read(fd[0], buf, 255);
        printf("[%d] read data success, data : %s\n", getpid(), buf);
        // close read-pipe
        close(fd[0]);
        printf("[%d] close read-pipe success\n", getpid());
    }
}