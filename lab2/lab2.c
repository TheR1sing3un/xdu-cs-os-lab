#include<stdio.h>
#include<pthread.h>
#include<unistd.h>
// sharedData is the data which can be shared from parent process to parent process's thread.
static int shareData = 1;
// this function is the start function of the thread we create. 
// We should assign this function's address when we create a thread by calling func: pthread_create.
void* thread_fn(void* args){
    printf("I'm a cute thread, I'm born with args: [%s]\n", args);
    printf("I can see my parent's data, it is %d !\n", shareData);
    int beforeData = shareData;
    shareData = 2;
    printf("And I'm a evil thread, I change my parents' data from %d to %d! HaHa\n", beforeData, shareData);
    return;
}
int main(){
    pthread_t tid;
    char args[10] = "ANewThread";
    // create a thread and pass a args to it
    if (!pthread_create(&tid, NULL, thread_fn, args)){
        //this create function return 0 means that creating a thread sucess!
        printf("[%d]I'm parent process, I've successfully created a thread %u\n", getpid(), tid);
        //using join to wait for thread
        pthread_join(tid,NULL);
        printf("[%d]My boy! You change my data!!!(shared data is %d now)\n", getpid(), shareData);
        return 0;
    }
    // if creating thread failed
    printf("[%d] Oh!!No!! The most failed thing in my life is to create a thread failed\n", getpid());
    return -1;
}