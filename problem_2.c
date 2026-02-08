#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <time.h>

int n, k; 
int* x; 
int m_sum = 0; 
int s_sum = 0; 
double s_norm; 

pthread_mutex_t m_sum_lock;

void generate_vector() {

	
	srand(time(NULL)); 

	
	for(int i = 0 ; i < n ; i++) {

		x[i] = (rand() % 100) + 1; 

	}

}

void compute_norm_single_threaded() {

	for(int i = 0 ; i < n ; i++) {

		s_sum += x[i] * x[i];

	}

	s_norm = sqrt((double)s_sum);

}

void* partial_sum(void* index) {

	int idx = (int)index;

	int p_sum = 0; 
	for(int i = idx ; i < n ; i += k) {

		p_sum += x[i] * x[i];

	}
	
	pthread_mutex_lock(&m_sum_lock);
	m_sum += p_sum;
	pthread_mutex_unlock(&m_sum_lock);

	printf("Thread %d resulted with partial sum: %d\n", idx + 1, p_sum);
	usleep(500000);
	pthread_exit(0);
}

int main() {

	
	printf("Enter values of n and k\n");
	scanf("%d%d", &n, &k);

	
	x = malloc(n * sizeof(int));

	generate_vector();


	pthread_t pool[k];
	
	
	for(int i = 0 ; i < k ; i++) {

		int* index = malloc(sizeof(int));
		*index = i;

		pthread_create(&pool[i], NULL, partial_sum, index);
	}

	for(int i = 0 ; i < k ; i++) {

		pthread_join(pool[i], NULL);
        
	}

	printf("Euclidean norm computed using multithreading: %6.2f\n", sqrt((double)m_sum));
	
	compute_norm_single_threaded();
	printf("Single-threaded result: %6.2f\n", s_norm);
}