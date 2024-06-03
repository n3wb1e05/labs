#include <iostream>
#include <pthread.h>
#include <stdlib.h>
#include <ctime>
#include <vector>
#include <string>
#include <unistd.h>
using namespace std;

pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_f=PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_e=PTHREAD_COND_INITIALIZER;

int buffer_size;
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
        pthread_mutex_lock(&mutex);// enter to critical section
        cout<<"I'm new producer!"<<endl;
        while(share_buffer.size()==buffer_size){
            pthread_cond_wait(&cond_f, &mutex);
        }
        pid_t numpv = gettid();
        int obj = create(100, 1000);
        time_t now = time(0);
        tm* ltm = localtime(&now);

        cout<<"I'm producer:"<<ltm->tm_hour<<":"<<ltm->tm_min<<":"<<ltm->tm_sec<<" - ID:"<<numpv<<"-I created object "<<obj<<endl;
        share_buffer.push_back(obj);
        pthread_cond_signal(&cond_e);
        pthread_mutex_unlock(&mutex); 
    } 
}

void *consomer(void *num)
{
    while(true){
        //check whether buffer is empty or not
        pthread_mutex_lock(&mutex);
        while(share_buffer.size()==0){
            pthread_cond_wait(&cond_e,&mutex);
        }
        pid_t id = gettid();
        // enter to critical section
        int el = share_buffer.front();
        use(el,id);
        vector<int>::iterator beg;
        beg = share_buffer.begin();
        share_buffer.erase(beg);
        pthread_cond_signal(&cond_f);
        pthread_mutex_unlock(&mutex);
    }
}
int main(int argc, char *argv[])
{
    if (argc!=4){
        cout<<"Please enter 3 arguments: ./lab8 arg1 arg2 arg3"<<endl;
        cout<<"arg1 - number of consomers\n"<<"arg2 - number of producers\n"<<"arg3 - size of buffer\n";
        return -1;
    }
    int num_cons = stoi(argv[1]);
    int num_prod = stoi(argv[2]);
    int buff_sz = stoi(argv[3]);
    int *pointer_to_buff_sz = &buffer_size;
    *pointer_to_buff_sz = buff_sz;
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
    pthread_mutex_destroy(&mutex);
    cout<<"Shop is closed!"<<endl;
}