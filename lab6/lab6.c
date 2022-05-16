#include<stdlib.h>
#include<stdio.h>
#include<sys/sem.h>
#include<sys/mman.h>
#include<unistd.h>
#include<sys/shm.h>
#include<sys/wait.h>

#define MAXSEM 5

int full_id, empty_id, mutex_id;

struct sembuf P,V;

typedef struct {
    int get;
    int set;
    int array[5];
    int sum;
}data;

int p (int sem_id) {
    P.sem_num = 0;
    P.sem_op = -1;
    P.sem_flg = SEM_UNDO;
    if (semop(sem_id, &P, 1) == -1) {
        perror("p error");
        return 0;
    }
    return 1;
}

int v (int sem_id) {
    V.sem_num = 0;
    V.sem_op = 1;
    V.sem_flg = SEM_UNDO;
    if (semop(sem_id, &V, 1) == -1) {
        perror("v error");
        return 0;
    }
    return 1;
}


union semun{
    int val;                        //cmd为SETVAL时，用于指定信号量值
    struct semid_ds *buf;            //cmd为IPC_STAT时或IPC_SET时生效
    unsigned short *array;            //cmd为GETALL或SETALL时生效
    struct seminfo *_buf;            //cmd为IPC_INFO时生效
};

int getval(int sem_id) {
    union semun u;
    return semctl(sem_id, 0, GETVAL, u);
}

int main(){
    pid_t producer;
    pid_t consumer_a;
    // set P and V
    P.sem_num = 0;
    P.sem_op = -1;
    P.sem_flg = SEM_UNDO;
    V.sem_num = 0;
    V.sem_op = 1;
    V.sem_flg = SEM_UNDO;
    int number;
    // create key
    key_t key0 = ftok(".", 11);
    key_t key1 = ftok(".", 12);
    key_t key2 = ftok(".", 13);
    // get three semaphore
    full_id = semget(key0, 1, IPC_CREAT);
    empty_id = semget(key1, 1, IPC_CREAT);
    mutex_id = semget(key2, 1, IPC_CREAT);
    // set semaphore value
    union semun arg;
    // set full-sem val
    arg.val = 0;
    if (semctl(full_id, 0, SETVAL, arg) == -1) {
        perror("semctl setval-full error");
        return -1;
    }
    // set empty-sem val
    arg.val = MAXSEM;
    if (semctl(empty_id, 0, SETVAL, arg) == -1) {
        perror("semctl setval-empty error");
        return -1;
    }
    // set mutex-sem val
    arg.val = 1;
    if (semctl(mutex_id, 0, SETVAL, arg) == -1) {
        perror("semctl setval-mutex error");
        return -1;
    }
    // create share memory
    // create a key
    key_t key3 = ftok(".", 14);
    if (key3 == -1) {
        printf("[%d] create key error\n", getpid());
        return -1;
    }
    // create key success
    // create a shared memory
    int s = shmget(key3, sizeof(data), IPC_CREAT);
    if (s == -1) {
        // create a shared memory error
        printf("[%d] create a shard memory error\n", getpid());
        return -1;
    }
    // create a shared memory success
    // connet its virtual address to this shared memory address
    char* address = (char *)shmat(s, NULL, SHM_EXEC);
    // read data from shared memory
    data* d = (data*)address;
    // we should init data first
    d->set = 0;
    d->get = 0;
    d->sum = 0;
    int j;
    for(j = 0; j < MAXSEM; j++) {
        d->array[j] = 0;
    }
    producer = fork();
    if (producer == -1) {
        printf("[%d] create producer process error\n", getpid());
        return -1;
    }
    if (producer) {
        // parent process
        // create consumerA process
        consumer_a = fork();
        if (consumer_a == -1) {
            printf("[%d] create consumerA process error\n", getpid());
            return -1;
        }
        if (consumer_a) {
            // consumerA' parent, we call it consumerB
            printf("[%d] create a consumerB success\n", getpid());
            while(1) {
                // p full, if now full-sem == 0, it will be blocked, because there is nothing to consume.
                p(full_id);
                // then p mutex-sem
                p(mutex_id);
                // critical area code
                if(d->get == 20){
                    // if all consumers have took data 100 times
                    v(mutex_id);
                    v(full_id);
                    break;
                }
                d->sum += d->array[(d->get) % MAXSEM];
                printf("consumer-B get number %d\n", d->array[(d->get) % MAXSEM]);
                d->get++;
                if (d->get == 20) {
                    printf("sum is %d\n", d->sum);
                    break;
                }
                // v mutex-sem and empty-sem
                v(mutex_id);
                // v empty, if now producer is blocked in p(empty_id), and now we v(empty_id), then producer can continue to produce because the consumer consume a procuct just now.
                v(empty_id);
                // sleep a while
                sleep(1);
            }
            printf("consumer-B quit\n");
        } else{
            // consumerA
            printf("[%d] create a consumerA success\n", getpid());
            while(1) {
                // p full, if now full-sem == 0, it will be blocked, because there is nothing to consume.
                p(full_id);
                // then p mutex-sem
                p(mutex_id);
                // critical area code
                if(d->get == 20){
                    // if all consumers have took data 100 times
                    v(mutex_id);
                    v(full_id);
                    break;
                }
                d->sum += d->array[(d->get) % MAXSEM];
                printf("consumer-A get number %d\n", d->array[(d->get) % MAXSEM]);
                d->get++;
                if (d->get == 20) {
                    printf("sum is %d\n", d->sum);
                    break;
                }
                // v mutex-sem and empty-sem
                v(mutex_id);
                // v empty, if now producer is blocked in p(empty_id), and now we v(empty_id), then producer can continue to produce because the consumer consume a procuct just now.
                v(empty_id);
                // sleep a while
                sleep(1);
            }
            printf("consumer-A quit\n");
        }
    } else {
        printf("[%d] create a producer success\n", getpid());
        int i;
        for(i = 0; i < 20; i++) {
            // produce process
            // p empty, if now empty-sem == 0, it will be blocked
            p(empty_id);
            p(mutex_id);
            printf("producer: produce %d\n", d->set);
            d->array[d->set % MAXSEM] = i + 1;
            d->set++;
            v(mutex_id);
            // v full
            v(full_id);
        }
        // sleep a while
        sleep(1);
        printf("producer quit\n");
    }
    sleep(5);
    if (producer && consumer_a) {    
        union semun n;
        semctl(full_id, 0, IPC_RMID, n);
        semctl(empty_id, 0, IPC_RMID, n);
        semctl(mutex_id, 0, IPC_RMID, n);
    }
}