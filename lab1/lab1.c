#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
int main() {
    //fork a process
    pid_t p = fork();
    if (p < 0) {
        printf("create process eroor\n");
        return;
    }
    if (p) {
        //p > 0 means that this process is parent process, so just wait for child process.
        pid_t childPid = waitpid(p, NULL, 0);
        printf("[%d]I'm parent process, my child process' pid is %d\n", getpid(), childPid);
        return;
    }else {
        // p = 0 means that this process is child process, so it need to read a file.
        printf("[%d]Hello, I'm child process, It' s my time to raed a damn file!\n", getpid());
        FILE* f = fopen("./lab1.txt", "r");
        // read 255 bytes once
        char buff[255];
        while(!feof(f)) {
            if (fgets(buff, 255, f) != NULL){
                printf("%s", buff);
            }
        }
        printf("\n");
        // don't forget to close the damn file!
        fclose(f);
        printf("[%d]Hello,I have finished my task about reading a file!\n", getpid());
        return;
    }
    
}