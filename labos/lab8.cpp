#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <ctime>
#include <vector>
#include <string>
using namespace std;

sem_t critical_p, critical_c;
sem_t full_prod; //0 is full, 1 - not full
sem_t empty_prod; //0 is empty, 1 - not empty
vector<int> share_buffer;
const int buffer_size = 10; 

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
int use(int b ,int id){
    pid_t PID;
    cout<<"I'm client:"<<time(NULL)<<" - ID:"<<id<<"- I got object - " << b <<endl;
    return 0;
}
void *producer(void *num)
{
    //check whether buffer is full or not
    sem_wait(&critical_p);// enter to critical section
    cout<<"I'm new producer!"<<endl;
    int obj;
    for(int i = 0; i<buffer_size; i++){
        sem_wait(&full_prod);
        obj = create(100, 1000);
        cout<<"I'm producer:"<<time(NULL)<<" - ID:"<<*(int *)num<<"-I created object "<<obj<<endl;
        share_buffer.push_back(obj);
        sem_post(&empty_prod);
    }
    //tell about not empty heap of objects(for consomers)
    sem_post(&critical_p);// exit from critical section   
}

void *consomer(void *num)
{
    //check whether buffer is empty or not
    sem_wait(&critical_c);
    sem_wait(&empty_prod);// enter to critical section
    int el = share_buffer.front();
    int a = *(int *)num;
    use(el,a);
    vector<int>::iterator beg;
    beg = share_buffer.begin();
    share_buffer.erase(beg);
    
    sem_post(&full_prod);//tell about used goods
    sem_post(&critical_c);// exit from critical section
}
int main(int argc, char *argv[])
{
    if (argc!=3){
        cout<<"Please enter 2 arguments: ./lab8 arg1 arg2 arg3"<<endl;
        cout<<"arg1 - buffersize\n"<<"arg2 - number of consomers\n"<<"arg3 - number of producers\n";
        return -1;
    }
    // int buffersize = stoi(argv[1]); 
    int num_cons = stoi(argv[1]);
    int num_prod = stoi(argv[2]);
    sem_init(&full_prod, 0, buffer_size);
    sem_init(&empty_prod, 0, 0);
    sem_init(&critical_c, 0 ,1);
    sem_init(&critical_p, 0 ,1);
    pthread_t consomers[num_cons];
    pthread_t producers[num_prod];
    int i;
    //creating threads
    for(i = 0; i<num_cons; i++){
        pthread_create(&consomers[i], NULL, consomer, (void *)&i);        
    }  
    
    for(i = 0; i<num_prod; i++){
        pthread_create(&producers[i], NULL, producer, (void *)&i);  
    }
    for(i = 0; i<num_cons; i++){
        pthread_join(consomers[i], NULL);
    }
    
    //joining threads
    for(i = 0; i<num_prod; i++){
        pthread_join(producers[i], NULL);
    }  
    
    cout<<"Shop is closed!"<<endl;
}