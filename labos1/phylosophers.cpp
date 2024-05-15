#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <string>

using namespace std;
const int NUM_PHILOSOPHERS = 5;
pthread_mutex_t forks[NUM_PHILOSOPHERS];

int time_s;
int plate[5];
chrono::time_point<std::chrono::system_clock> time_start[5];
pthread_mutex_t exit_m;
mutex print_status;

string ret_time(){

}

string printStateForks()
{
    
    string state = "";
    for(int j = 0; j<5; j++){
        if(pthread_mutex_trylock(&forks[j])==0){
            pthread_mutex_unlock(&forks[j]);
            state=state+"O";
        }else{
            state=state+"X";
        }  
    }
    return state;
}

void philosopher(int id) {
    using seconds = std::chrono::seconds;
    int left_fork = id;
    int right_fork = (id + 1) % NUM_PHILOSOPHERS;
    time_start[id] = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds;
    while (true) {
         
        // print_status.lock();
        cout << "Philosopher " << id << " is thinking. State forks:" <<printStateForks()<< endl;
        this_thread::sleep_for(chrono::seconds(1));
        // print_status.unlock();
        
        if((pthread_mutex_trylock(&forks[left_fork])==0) && (pthread_mutex_trylock(&forks[right_fork])==0)){
            if(plate[id]==0){
                cout<< "Philosopher " << id <<".I ate! Thank you!"<<endl;
                pthread_mutex_unlock(&forks[right_fork]);
                pthread_mutex_unlock(&forks[left_fork]);  
                pthread_exit(NULL);
            }
            plate[id]--;
            cout<<"How many i have spagetti:"<<plate[id]<<endl;
            
            cout << "Philosopher " << id << " is eating with "<<left_fork<<" left fork and "<<right_fork<<" right fork." <<"State forks:"<< printStateForks()<< endl;
            this_thread::sleep_for(chrono::milliseconds(1500)); 
            pthread_mutex_unlock(&forks[right_fork]);
            pthread_mutex_unlock(&forks[left_fork]);    
        }    
        else{
            if(pthread_mutex_unlock(&forks[right_fork])){
                cout<<"Philosopher "<<id<<".Cannot take right fork: "<<right_fork<<endl;
                cout<<"Philosopher "<<id<<". Left fork is free:"<<left_fork<<endl;
            }
            else if((pthread_mutex_unlock(&forks[left_fork])==0)){
                cout<<"Philosopher "<<id<<".Cannot take left fork: "<<left_fork<<endl;
                cout<<"Philosopher "<<id<<". Right fork is free:"<<right_fork<<endl;    
            }
            pthread_mutex_lock(&exit_m);
            elapsed_seconds = chrono::duration_cast<seconds>(std::chrono::system_clock::now() - time_start[id]);
            if(elapsed_seconds.count()>time_s){
                cout<<"Phylosopher "<<id<<" was faint!"<<endl;
                pthread_mutex_unlock(&exit_m);
                pthread_exit(NULL);
            }
            pthread_mutex_unlock(&exit_m);
            
        }        
    }
}

int main(int argc, char *agrv[]) {
    if(argc!=3){
        cout<<"./phyl <time> <buffer_size>"<<endl;
        return -1;
    }
    int *timep = &time_s;
    *timep = stoi(agrv[1]);

    int *bp = plate;
    for(int a = 0;a<5;bp++){
        *bp=stoi(agrv[2]);
        a++;
    }
    thread philosophers[NUM_PHILOSOPHERS];
    // Create threads for each philosopher
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        philosophers[i] = thread(philosopher, i);
    }

    // Join threads (should never reach this point in this example)
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        philosophers[i].join();
    }

    return 0;
}