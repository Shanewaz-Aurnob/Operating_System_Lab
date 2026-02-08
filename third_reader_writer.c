#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define NUM_WRITERS 3
#define NUM_READERS 5


#define red "\e[0;31m"
#define green "\e[0;32m"
#define yellow "\e[0;33m"
#define blue "\e[0;34m"
#define magenta "\e[0;35m"
#define cyan "\e[0;36m"

char colreset[] = "\e[0m";
char *colorname[] = {red, green, yellow, blue, magenta, cyan};

int data; 

pthread_mutex_t rmutex        = PTHREAD_MUTEX_INITIALIZER, 
				service_queue = PTHREAD_MUTEX_INITIALIZER,
				resources     = PTHREAD_MUTEX_INITIALIZER;

int read_count;; 

void *reader(void *param); 
void *writer(void *param); 

int main()
{
	pthread_t tid_r[NUM_READERS], tid_w[NUM_WRITERS]; 
	 
	read_count = 0; 
	int i;
	for (i = 0; i < NUM_WRITERS; i++) {
		pthread_create(&tid_w[i], NULL, writer, (void *) (&i));
	}
	
	for (i = 0; i < NUM_READERS; i++) {
		pthread_create(&tid_r[i], NULL, reader, (void *) (&i));
	}
	
	for (i = 0; i < NUM_WRITERS; i++) {
		pthread_join(tid_w[i], NULL); 
	}
	
	for (i = 0; i < NUM_READERS; i++) {
		pthread_join(tid_r[i], NULL); 
	}
	
} 

void *reader(void *param) {
	int idx = *((int *) param); 
	char *col = colorname[idx]; 
	printf("%sEntering reader %d %s\n", col, idx, colreset); 
	while (true) {
		pthread_mutex_lock(&service_queue); 
		pthread_mutex_lock(&rmutex); 
		read_count++;
		if (read_count == 1) 
			pthread_mutex_lock(&resources); 
		pthread_mutex_unlock(&service_queue); 
		pthread_mutex_unlock(&rmutex); 
		sleep(rand() % 2);
		printf("%sReader %d read data = %d<current no. reader = %d>%s\n", col, idx, data, read_count, colreset); 
		pthread_mutex_lock(&rmutex); 
		read_count--; 
		if (read_count == 0) 
			pthread_mutex_unlock(&resources); 
		pthread_mutex_unlock(&rmutex);
		sleep(rand() % 3);
	}
}


void *writer(void *param) {
	int idx = *((int *) param); 
	char *col = colorname[idx]; 
	printf("%sEntering writer %d %s\n", col, idx, colreset);
	while (true) {
		pthread_mutex_lock(&service_queue); 
		pthread_mutex_lock(&resources); 
		pthread_mutex_unlock(&service_queue); 
		sleep(rand() % 3);
		int t = data; 
		data = 100 + rand() % 100;
		printf("%sWriter %d made data from %d to %d%s\n", col, idx, t, data, colreset); 
		pthread_mutex_unlock(&resources); 
		sleep(rand() % 2);

	}
}