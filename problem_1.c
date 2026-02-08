#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_THREADS 2

int shared_variable = 0;

void *updateVariable(void *threadID) {
    long tid = (long)threadID;
    
    
    if (tid == 0) {
        
        shared_variable += 5;
        printf("Thread %ld updated the variable to: %d\n", tid, shared_variable);
        sleep(1); 
    } else {
        
        shared_variable -= 3;
        printf("Thread %ld updated the variable to: %d\n", tid, shared_variable);
        sleep(1); 
    }

    pthread_exit(NULL);
}

int main() {
    pthread_t threads[NUM_THREADS];
    int rc;
    long t;

    for (t = 0; t < NUM_THREADS; t++) {
        printf("Creating thread %ld\n", t);
        rc = pthread_create(&threads[t], NULL, updateVariable, (void *)t);
        if (rc) {
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            return -1;
        }
    }


    for (t = 0; t < NUM_THREADS; t++) {
        pthread_join(threads[t], NULL);
    }

    printf("Final value of the shared variable: %d\n", shared_variable);

    pthread_exit(NULL);
}
