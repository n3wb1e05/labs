#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <ctime>
#include <vector>
#include <string>
#include <unistd.h>
using namespace std;

sem_t lock;
sem_t full_prod; //0 is full, 1 - not full
sem_t empty_prod; //0 is empty, 1 - not empty
vector<int> share_buffer;

int random(int min, int max){   
    if(min>max){
        swap(min, max);
    }
    int numbers[max-min];
    for(int i = min; i<max; i++){
        numbers[i-min] = i;
    }
    //select random
    srand(clock());
    int header_function = rand();
    int rand = numbers[header_function%max];
    return rand;
}
int create(int a, int b){
    int rand_v = random(a,b);
    int res = rand_v%a;
    return res;
}
int use(int obj ,pid_t id){
    time_t now = time(0);
    tm* ltm = localtime(&now);
    cout<<"I'm client:"<<ltm->tm_hour<<":"<<ltm->tm_min<<":"<<ltm->tm_sec<<" - ID:"<<id<<"- I got object - " << obj <<endl;
    return 0;
}
void *producer(void *nump)
{
    while(true){
        sem_wait(&lock);// enter to critical section
        sem_wait(&full_prod);
        cout<<"I'm new producer!"<<endl;
        pid_t numpv = gettid();
        
        int obj= create(100, 1000);
        time_t now = time(0);
        tm* ltm = localtime(&now);

        cout<<"I'm producer:"<<ltm->tm_hour<<":"<<ltm->tm_min<<":"<<ltm->tm_sec<<" - ID:"<<numpv<<"-I created object "<<obj<<endl;
        share_buffer.push_back(obj);
        //tell about not empty heap of objects(for consomers)
        sem_post(&empty_prod);
        sem_post(&lock);// exit from critical section 
    }
}

void *consomer(void *num)
{
    while(true){
        sem_wait(&empty_prod);
        //check whether buffer is empty or not
        sem_wait(&lock);
        pid_t id = gettid();
        // enter to critical section
        int el = share_buffer.front();
        use(el,id);
        vector<int>::iterator beg;
        beg = share_buffer.begin();
        share_buffer.erase(beg);
        
        sem_post(&full_prod);//tell about used goods
        sem_post(&lock);// exit from critical section
    }
}
int main(int argc, char *argv[])
{
    if (argc!=4){
        cout<<"Please enter 3 arguments: ./lab8 arg1 arg2 arg3"<<endl;
        cout<<"arg1 - buffersize\n"<<"arg2 - number of consomers\n"<<"arg3 - number of producers\n";
        return -1;
    }
    int buffersize = stoi(argv[1]); 
    int num_cons = stoi(argv[1]);
    int num_prod = stoi(argv[2]);
    sem_init(&full_prod, 0, buffersize);
    sem_init(&empty_prod, 0, 0);
    sem_init(&lock, 0 ,1);
    pthread_t consomers[num_cons];
    pthread_t producers[num_prod];
    int i,j;
    //creating threads
    for(i = 0; i<num_cons; i++){
        pthread_create(&consomers[i], NULL, consomer, NULL);        
    }  
    
    for(j = 0; j<num_prod; j++){
        pthread_create(&producers[j], NULL, producer, NULL);  
    }
    for(i = 0; i<num_cons; i++){
        pthread_join(consomers[i], NULL);
    }
    
    //joining threads
    for(j = 0; j<num_prod; j++){
        pthread_join(producers[j], NULL);
    }  
    
    cout<<"Shop is closed!"<<endl;
}