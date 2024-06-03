#include <iostream> 
#include <thread> 
#include <unistd.h>
using namespace std;

void *work_thread (void *args) {
    for(int i = 0; i< 10; i++)
        printf("%d - I'm second thread\n",i);
}
int main(){
    for(int i = 0; i<10; i++){
    printf("%d - I'm main thread\n",i);

    }

    printf("\n");

    pthread_t th;
    pthread_create(&th, NULL, work_thread, NULL);
    pthread_join(th, NULL);
}
