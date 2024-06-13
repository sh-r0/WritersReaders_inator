#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdint.h>

int32_t readersNum_g;
int32_t writersNum_g;

int32_t readersWaiting_g;
int32_t writersWaiting_g;

//fflush(stdout)

void* readerFunc(void* _args) {

    return NULL;
} 

void* writerFunc(void* _args) {

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

    pthread_t readerThreads[readersNum_g];
    pthread_t writerThreads[writersNum_g];

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