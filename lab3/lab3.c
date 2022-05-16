#include<unistd.h>
#include<signal.h>
#include<wait.h>
#include<sys/types.h>
#include<stdio.h>

// receive signal and handle it
void signal_handler(int signal) {
    pid_t pid;
    int status;
    // receive signal
    // param -1 means that wait any child process
    // param WNOHANG means that if any child process still running, the function will directly return 0
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        printf("[%d]child %d died: %d\n", getpid(), pid, WEXITSTATUS(status));
        printf("[%d]hi, parent process received SIGCHLD signal successfunlly!\n", getpid());
    }
    return;
}

int main() {
    // create a child process
    pid_t p = fork();
    if (p < 0) {
        printf("create process eroor\n");
        return;
    }
    if (p) {
        // p > 0 means that this process is parent process
        // call signal_handler
        // SIGCHLD means that when child process exit, it will send a signal named SIGCHLD.
        signal(SIGCHLD, signal_handler);
        pause();
    }else {
        // p = 0 means that this process is child process
        // print
        printf("[%d]Hello, I'm child process, my parent is %d\n", getpid(), getppid());
        sleep(1);
        printf("[%d]sleep 1s. Now, I'm child process, my parent is %d\n", getpid(), getppid());
        sleep(1);
        printf("[%d]sleep 1s again. Now, I'm child process, my parent is %d\n", getpid(), getppid());
        return;
    }
}