/* A demo code that employs POSIX threads and mutex
 * in solving the readers-writer's problem, a classic IPC problem.
 * Written by Kazi Ashrafuzzaman for a session in the course CSE 512 at CSE, CU
 */
#include<pthread.h>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>

#define NUM_READERS 5
#define NUM_WRITERS 3
#define MAX_NUM_WRITES 7
/* We are terminating based on this. Other forms of termination possible. */

#define red "\e[0;31m"
#define green "\e[0;32m"
#define yellow "\e[0;33m"
#define blue "\e[0;34m"
#define magenta "\e[0;35m"
#define cyan "\e[0;36m"

char colreset[] = "\e[0m";
char *colorname[] = {red, green, yellow, blue, magenta, cyan};

int data; /* this data is shared by the writer and the readers */

int count_writes; /* Number of writes made so far. Remember, there is only _one_ writer */

int count_readers;

/* Readers versus writer mutual exlusion lock.  Not among the readers though. */
pthread_mutex_t wlock = PTHREAD_MUTEX_INITIALIZER;

/* Mutual exclusion lock among readers  while count_readers gets updated. */
pthread_mutex_t ru_lock = PTHREAD_MUTEX_INITIALIZER;

void *reader(void *param);
void *writer(void *param);

int main(int argc, char *argv[])
{
    pthread_t tid_w[NUM_WRITERS];
    pthread_t tid_r[NUM_READERS];

    data = 0;
    count_readers = 0;
    count_writes = 0;
    int reader_idx[NUM_READERS];
    int writer_idx[NUM_WRITERS];

    //pthread_create( &tid_w, NULL, writer, NULL);
    for(int j=0; j< NUM_READERS; j++)
    {
        reader_idx[j] = j+1;
        pthread_create( &tid_r[j], NULL, reader, &reader_idx[j]);
    }

    for(int j=0; j< NUM_WRITERS; j++)
    {
        writer_idx[j] = j+1;
        pthread_create( &tid_w[j], NULL, writer, &writer_idx[j]);
    }


    //pthread_join(tid_w,NULL);
    for(int j=0; j< NUM_READERS; j++)
    {
        pthread_join(tid_r[j],NULL);
    }
    
    for(int j=0; j< NUM_WRITERS; j++)
    {
        pthread_join(tid_w[j],NULL);
    }
}

void *writer(void *param)
{
    char *clr = colorname[0]; // Single writer and it prints in red
    //printf("%sWriter started ... %s\n", clr, colreset);
    int *idx = (int*) param;
    while(true) {
        pthread_mutex_lock(&wlock);
        int t = data;
        data = 100+rand()%100;
        count_writes++;
        printf("%sWriter %d replaced data %d with %d %s\n", clr, *idx, t, data, colreset);
        unsigned sleep_t = 50000+rand()%9000;
        usleep(sleep_t);
        pthread_mutex_unlock(&wlock);

        sleep_t = 100000+rand()%300000;
        usleep(sleep_t);

        if(count_writes >= MAX_NUM_WRITES)
        {
            printf("%s....Writer %d closing %s\n", clr, *idx, colreset);
            pthread_exit(0);
        }
    }
}

void *reader(void *param)
{
    int *idx = (int*) param;
    // The following is done to allow for more threads than colors (5) defined as macros above.
    // Different colors ensured for the threads, even if not as many as there are threads.
    // For instance, each color gets used twice if you define NUM_READERS to be 10.
    //char *clr = colorname[1+(*idx)%5];
    char *clr = colorname[1];

    printf("%sReader %d called\n", clr, *idx);

    while(true) {
        pthread_mutex_lock(&ru_lock);
        if(count_readers==0)
            pthread_mutex_lock(&wlock);
        count_readers++;
        printf("%sReader %d read data %d <reader-count=%d now>%s\n", clr, *idx, data, count_readers, colreset);
        pthread_mutex_unlock(&ru_lock);

        unsigned sleep_t = 50000+rand()%90000;
        usleep(sleep_t);

        pthread_mutex_lock(&ru_lock);
        count_readers--;
        printf("%sReader %d stopped reading. <reader-count=%d now>%s\n", clr, *idx, count_readers, colreset);
        if(count_readers==0)
            pthread_mutex_unlock(&wlock);
        pthread_mutex_unlock(&ru_lock);

        sleep_t = 100000+rand()%60000;
        usleep(sleep_t);

        if(count_writes >= MAX_NUM_WRITES)
        {
            printf("%s....Reader %d closing %s\n", clr, *idx, colreset);
            pthread_exit(0);
        }
    }
}
