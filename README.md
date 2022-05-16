# 西电操作系统实验

## 实验一

### 实验题目

创建进程

### 实验目的

学会通过基本的 Windows 或者 Linux 进程控制函数，由父进程 创建子进程，并实现父子进程协同工作。

### 实验环境

VC++6.0 或 Linux 操作系统

### 实验内容

创建两个进程，让子进程读取一个文件，父进程等待子进程读取 完文件后继续执行，实现进程协同工作。 进程协同工作就是协调好两个进程，使之安排好先后次序并以此 执行，可以用等待函数来实现这一点。当需要等待子进程运行结束 时，可在父进程中调用等待函数。

### 实验程序及分析

> 代码如下，分析在注释中

```c
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
```

### 实验截图

![image-20220515112211888](https://ther1sing3un-personal-resource.oss-cn-beijing.aliyuncs.com/typora/images/image-20220515112211888.png)

### 实验心得体会

感觉不错，做lab比看书有意思

---

## 实验二

### 实验题目

线程共享进程数据

### 实验目的

了解线程与进程之间的数据共享关系。创建一个线程，在线程中 更改进程中的数据。

### 实验环境

VC++6.0 或者 Linux 操作系统

### 实验内容

在进程中定义全局共享数据，在线程中直接引用该数据进行更改 并输出该数据。

### 实验程序及分析

> 代码如下，分析在注释里

```c
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
        //usirng join to wait for thread
        pthread_join(tid,NULL);
        printf("[%d]My boy! You change my data!!!(shared data is %d now)\n", getpid(), shareData);
        return 0;
    }
    // if creating thread failed
    printf("[%d] Oh!!No!! The most failed thing in my life is to create a thread failed\n", getpid());
    return -1;
}
```

### 实验截图

![image-20220515115857121](https://ther1sing3un-personal-resource.oss-cn-beijing.aliyuncs.com/typora/images/image-20220515115857121.png)

### 实验心得体会

线程是可以从进程那里共享到数据的，并且可以对其进行访问和修改。这告诉我们有时候败家子会毁了父母的家业的！

---

## 实验三

### 实验题目

信号通信

### 实验目的

利用信号通信机制在父子进程及兄弟进程间进行通信。

### 实验环境

VC++6.0 或者 Linux 操作系统

### 实验内容

父进程创建一个有名事件，由子进程发送事件信号，父进程获取 事件信号后进行相应的处理。

### 实验程序及分析

> 分析见注释

```c
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
        // p > 0 means that this process is parentprocess
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
```

### 实验截图

![image-20220515130845253](https://ther1sing3un-personal-resource.oss-cn-beijing.aliyuncs.com/typora/images/image-20220515130845253.png)

### 实验心得体会

很不错。

---

## 实验四

### 实验题目

匿名管道通信

### 实验目的

学习使用匿名管道在两个进程之间建立通信

### 实验软件环境

VC++6.0 或者 Linux 操作系统

### 实验内容

分别建立名为 Parent 的单文档应用程序和 Child 的单文档应用程 序作为父子进程，由父进程创建一个匿名管道，实现父子进程向匿 名管道写入和读取数据。

### 实验程序及分析

> 分析见注释

```c
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
```

### 实验截图

![image-20220515135016415](https://ther1sing3un-personal-resource.oss-cn-beijing.aliyuncs.com/typora/images/image-20220515135016415.png)

### 实验心得体会

挺好的，能通信就是因为fork了一样的文件描述符，指向同一个匿名管道。

---

## 实验五

### 实验题目

命名匿名管道通信

### 实验目的

学习使用命名匿名管道在多进程之间建立通信

### 实验环境

VC++6.0 或者 Linux 操作系统

### 实验内容

建立父子进程，由父进程创建一个命名匿名管道，由子进程向 命名管道写入数据，由父进程从命名管道读取数据。

### 实验程序及分析

```c
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
```

### 实验截图

![image-20220515161655080](https://ther1sing3un-personal-resource.oss-cn-beijing.aliyuncs.com/typora/images/image-20220515161655080.png)

### 实验心得体会

很不错，通过命名管道是可以不止父子/兄弟进程之间通信，任意进程都可以哦。

---

## 实验六

### 实验题目

信号量实现进程同步

### 实验目的

进程同步是操作系统多进程/多线程并发执行的关键之一，进程 同步是并发进程为了完成共同任务采用某个条件来协调他们的活 动，这是进程之间发生的一种直接制约关系。本次试验是利用信号 量进行进程同步。

### 实验环境

VC++或者 Linux 操作系统

### 实验内容

- 生产者进程生产产品，消费者进程消费产品。
- 当生产者进程生产产品时，如果没有空缓存区可以用，那么生产者进程必须等待消费者进程释放一个缓存区。
- 当消费者进程消费产品时，如果缓存区中没有产品，那么消费者进程将被阻塞，直到新的产品被生产出来。

### 实验程序及分析

```c
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
```

### 实验截图

![image-20220516180924372](https://ther1sing3un-personal-resource.oss-cn-beijing.aliyuncs.com/typora/images/image-20220516180924372.png)

### 实验心得体会

生产者消费者模型，最重要的是什么时候阻塞，也就是需要一个值为缓存区大小的信号量，来阻塞生产者在缓存区满的时候的生产操作；并且需要一个值为0的信号量，来阻塞缓存区空的时候的消费者消费问题。以及注意保护共享内存，不要出现进程同时访问临界区的情况，这也就是为什么需要一个信号量值为1的mutex，也就是互斥锁的原因了。别的就没啥了。

---

## 实验七

### 实验题目

共享内存实现进程通信

### 实验目的

利用共享内存解决读写者问题。要求写者进程创建一个共享主存，并向其中写入数据，读者进程随后从该共享主存区访问数据。

### 实验软件环境

### 实验内容

### 实验程序及分析

> write

```c
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
    p->age = 19;
    p->name[0] = 'l';
    p->name[1] = 'c';
    p->name[2] = 'y';
    printf("[%d] write data. Data: name = %s, age = %d\n", getpid(), p->name, p->age);
    // disconnet the shared memory
    shmdt(address);
}

```

> read

```c
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

```

### 实验截图

![image-20220515181922633](https://ther1sing3un-personal-resource.oss-cn-beijing.aliyuncs.com/typora/images/image-20220515181922633.png)

### 实验心得体会

共享内存。其实就是一片内存区域，咱几个进程一起将它映射到本地的虚拟地址空间，也就是这部分内存空间我们是访问到一样的，因此可以通信。

---

