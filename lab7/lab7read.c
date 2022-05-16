#include<sys/ipc.h>
#include<sys/shm.h>
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>

typedef struct 
{
    char name[3];
    int age;
} people;

int main() {
    // create a key
    key_t k = ftok(".", 1);
    if (k == -1) {
        printf("[%d] create key error\n", getpid());
        return -1;
    }
    // create key success
    // create a shared memory
    int s = shmget(k, sizeof(people), IPC_CREAT);
    if (s == -1) {
        // create a shared memory error
        printf("[%d] create a shard memory error\n", getpid());
        return -1;
    }
    // create a shared memory success
    // connet its virtual address to this shared memory address
    char* address = (char *)shmat(s, NULL, SHM_EXEC);
    // read data from shared memory
    people* p = (people*)address;
    printf("[%d] read data. Data: name = %s, age = %d\n", getpid(), p->name, p->age);
    // disconnet the shared memory
    shmdt(address);
}
