#define n_files 2

#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <random>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <time.h>

using namespace std;

struct sembuf lock_sem = {0,-1,0};
struct sembuf unlock_sem = {0,1,0};
const char *pathname = "/home/kyrylshabanv";


char* get_random_task(char* shmem) {
    std::random_device rd;
    std::mt19937 gen(rd());
    int prognumber = gen() % 2;
    return (shmem + prognumber * 34);
}

/*  
    Function for shared memory's segment
*/
//write and read
char* write_shm(char *word, char *address_shm, char n)
{
    for(int i = 0;i<n;i++){
        *address_shm = word[i];
        address_shm++;
    }
    return ++address_shm;
}

/*
    Semaphore operations
*/
void set_value(int id, int num_s, int n){
    union semun {
        int val;
        struct semid_ds *buf;
        unsigned short  *array;
        struct seminfo  *__buf;  
        } args;
        
    args.val = n;
    semctl(id, num_s, SETVAL, args);
}

struct message_struct{
    long mtype;   
    char mtext[100]; 
};

int main()
{
    key_t share_seg_mem = ftok(pathname,1),
    key_sem = ftok(pathname,2),
    key_mesqueue = ftok(pathname,3);
    char *ex_str = "/home/kyrylshabanv/code/lab9/lab7";
    int len = sizeof(ex_str);

    int id_shm,id_m,id_s;
    
    pid_t proc;
    for(int i = 0; i<3;i++){
        proc = fork();
        if(proc==-1){
            perror("fork");
            break;
        }else if(proc==0){
            break;
        }
    }
    struct message_struct snd_m;
    if(proc==0){
        cout<<"I'm process "<<getpid()<<".My father:"<<getppid()<<endl;
        sleep(1);
        char *alloc_seg;
        while((id_shm = shmget(share_seg_mem,len,0664))==-1){
            perror("open shm child:");
            sleep(2);
        }

        if(*(int *)(alloc_seg = (char *)shmat(id_shm,NULL,0))==-1){
            perror("shmat child error");
            return -1;
        }
        key_sem = ftok(pathname,2);
        int id_s;
        while((id_s = semget(key_sem,1, 0664))==-1){
            perror("semget child");
            sleep(2);
        }
        string mess;
        time_t time_n;
        tm* ltm;
        while((id_m = msgget(key_mesqueue,0664))==-1){
            perror("msgget");
            sleep(2);
        }
        for(int i = 0; i<5; i++){
            
            //choose random value
            semop(id_s,&lock_sem,1);
            
            pid_t for_child = fork();
            if(for_child==0){
                time_n = time(0);
                ltm = localtime(&time_n);
                char file[len];
                strcpy(file,get_random_task(alloc_seg));
                mess = "Time to start:"+to_string(ltm->tm_hour)+":"+to_string(ltm->tm_min)+":"+to_string(ltm->tm_sec)+".PPID:" +to_string(getppid())+"; execute:"+ file;
                snd_m.mtype = 1;
                strcpy(snd_m.mtext, mess.c_str()); 
                sleep(1);
                msgsnd(id_m,&snd_m,sizeof(snd_m.mtext),0);
                if(execl(file,file,NULL,NULL)==-1){
                    perror("fork child");
                }
            }
            else{
                if(wait(NULL))
                    semop(id_s,&unlock_sem,1);
            }
            
        }
    }
    //parent process
    else{
        cout<<"I`m father:"<<getpid()<<endl;
        char *alloc_shm;

        char *filenames[] = {
            "/home/kyrylshabanv/code/lab9/lab7",
            "/home/kyrylshabanv/code/lab9/phyl"
        };

        if((id_shm=shmget(share_seg_mem,len*2,(IPC_CREAT|0664)|IPC_EXCL))==-1){
            id_shm=shmget(share_seg_mem,len*2,0664);
        }

        if((id_s=semget(key_sem,1,(IPC_CREAT|0664)|IPC_EXCL))==-1){
            perror("semget error");
            id_s=semget(key_sem,1,0664);
        }
        set_value(id_s,0,1); //binary sem range (0 or 1)

        if((id_m=msgget(key_mesqueue,(IPC_CREAT|0664)))==-1){
            perror("msgget error");
            return -1;
        }

        if(*(int *)(alloc_shm = (char *)shmat(id_shm,NULL,0))==-1){
            perror("shmat error");
            return -1;
        }
        //copy filenames to segment memory
        int n;
        char* tpointer = alloc_shm;
        printf("alloc_sgm:0x%x\n", alloc_shm);
        for(int i = 0; i<n_files; i++){
            n = strlen(filenames[i]);
            alloc_shm = write_shm(filenames[i],alloc_shm,n); 
        }
        alloc_shm = tpointer; 

        struct message_struct mes_rcv;
        
        wait(NULL);

        for(int i = 0;i<15;i++){
            msgrcv(id_m,&mes_rcv,sizeof(mes_rcv.mtext),1,0);
            cout<<"I`m father process "<< getpid() <<".I got it:"<<mes_rcv.mtext<<endl;
        }
        cout<<"Removing shared memory segment, semaphore, message queue"<<endl;
        if(shmctl(id_shm,IPC_RMID,0)==-1){
            perror("shmctl");
        }
        if(semctl(id_s,1,IPC_RMID)==-1){
            perror("semctl");
        }
        if(msgctl(id_m,IPC_RMID,0)==-1){
            perror("msgctl");
        }
    }
}