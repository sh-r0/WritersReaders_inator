#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

int32_t readersNum_g;
int32_t writersNum_g;

int32_t readersWaiting_g;
int32_t writersWaiting_g;

int32_t readersInLib_g;
int32_t writersInLib_g;

pthread_mutex_t mutex_g;

pthread_cond_t readersCond_g;
pthread_cond_t writersCond_g;

// Program 1 - zaglodzenie pisarzy

//fflush(stdout)

//inline
void printState() {
    printf("Waiting: %i readers %i writers\nIn: %i readers %i writers\n\n", readersWaiting_g, writersWaiting_g, readersInLib_g, writersInLib_g);
    fflush(stdout);
    return;
}

//inline
void randSleep(){
    usleep(rand()%1000000); // 0-1s
    return;
}

void* readerFunc(void* _args) {
    srand(time(0));
    while(true) { 
        pthread_mutex_lock(&mutex_g);
        if(writersWaiting_g > 0 || writersInLib_g > 0) {
            readersWaiting_g++;
            printState();
            pthread_cond_wait(&readersCond_g, &mutex_g);
            readersWaiting_g--;
            readersInLib_g++;
            printState();
        }
        else {
            readersInLib_g++;
            printState();
        }
        pthread_mutex_unlock(&mutex_g);

        randSleep();

        pthread_mutex_lock(&mutex_g);
        readersInLib_g--;
        printState();
        if(readersInLib_g == 0) {
            pthread_cond_signal(&writersCond_g);
        } 
        pthread_mutex_unlock(&mutex_g);
        randSleep();
    }
    return NULL;
} 

void* writerFunc(void* _args) {
    srand(time(0));
    while(true) {
        pthread_mutex_lock(&mutex_g);
        while (readersInLib_g != 0 || writersInLib_g != 0) {
            writersWaiting_g++;
            pthread_cond_wait(&writersCond_g, &mutex_g);
            writersWaiting_g--;
        }
        writersInLib_g++;
        printState();
        pthread_mutex_unlock(&mutex_g);

        randSleep();
        
        pthread_mutex_lock(&mutex_g);
        writersInLib_g--;
        printState();
        if(writersWaiting_g != 0) {
            pthread_cond_signal(&writersCond_g);
        }
        else {
            pthread_cond_broadcast(&readersCond_g);
        }
        pthread_mutex_unlock(&mutex_g);
        randSleep();
    }
    return NULL;
} 

int32_t main(int32_t _argc, char** _argv){
    if(_argc < 3) {
        printf("Need more parameters!\nPass number of writers and readers");
        return -1;
    }
    writersNum_g = atoi(_argv[1]);
    readersNum_g = atoi(_argv[2]);
    
    if (writersNum_g == 0 || readersNum_g == 0) {
        printf("Erorr parsing numbers of readers/writers!\nClosing!\n");
        return -1;
    }

    writersInLib_g = 0, readersInLib_g = 0;
    writersWaiting_g = 0, readersWaiting_g = 0;

    pthread_mutex_init(&mutex_g, NULL);
    pthread_cond_init(&readersCond_g, NULL);
    pthread_cond_init(&writersCond_g, NULL);

    pthread_t writerThreads[writersNum_g];
    pthread_t readerThreads[readersNum_g];
    
    for(size_t i = 0; i < readersNum_g;i++){
        pthread_create(readerThreads+i, NULL, readerFunc, NULL);
    }
    for(size_t i = 0; i < writersNum_g;i++){
        pthread_create(writerThreads+i, NULL, writerFunc, NULL);
    }

    for(size_t i = 0; i < readersNum_g;i++){
        pthread_join(readerThreads[i], NULL);
    }
    for(size_t i = 0; i < writersNum_g;i++){
        pthread_join(writerThreads[i], NULL);
    }

    printf("Ended gracefully!\n");
    return 0;
}