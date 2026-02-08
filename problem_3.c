#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define MAX_DIM 10

typedef struct {
    int **M;
    int **N;
    int **result;
    int rows;
    int cols;
    int thread_id;
   } 
   
   ThreadData;

int** allocateMatrix(int rows, int cols) {

    int **matrix = (int **)malloc(rows * sizeof(int *));
    for (int i = 0; i < rows; i++) {

        matrix[i] = (int *)malloc(cols * sizeof(int));
      }
    return matrix;
 }

void generateRandomMatrix(int **matrix, int rows, int cols) {

    for (int i = 0; i < rows; i++) {

        for (int j = 0; j < cols; j++) {

            matrix[i][j] = rand() % 11; 

           }
      }
   }


void *multiplyColumn(void *arg) {

    ThreadData *data = (ThreadData *)arg;

    for (int i = 0; i < data->rows; i++) {

        int sum = 0;

        for (int j = 0; j < data->cols; j++) {

            sum += data->M[i][j] * data->N[j][data->thread_id];
        }
        data->result[i][data->thread_id] = sum;
    }

    pthread_exit(NULL);

}

int main() {

    int l, m, n;

    printf("Enter dimensions (l, m, n): ");
    scanf("%d %d %d", &l, &m, &n);

    if (l < 1 || l > MAX_DIM || m < 1 || m > MAX_DIM || n < 1 || n > MAX_DIM) {

        printf("Invalid dimensions.\n");
        return 1;

     }

    
    int **M = allocateMatrix(l, m);
    int **N = allocateMatrix(m, n);
    int **result = allocateMatrix(l, n);


    generateRandomMatrix(M, l, m);
    generateRandomMatrix(N, m, n);


    pthread_t threads[n];
    ThreadData thread_data[n];

    for (int i = 0; i < n; i++) {

        thread_data[i].M = M;
        thread_data[i].N = N;
        thread_data[i].result = result;
        thread_data[i].rows = l;
        thread_data[i].cols = m;
        thread_data[i].thread_id = i;

        pthread_create(&threads[i], NULL, multiplyColumn, (void *)&thread_data[i]);
     }

    for (int i = 0; i < n; i++) {

        pthread_join(threads[i], NULL);
     }

    printf("Matrix M:\n");

    for (int i = 0; i < l; i++) {

        for (int j = 0; j < m; j++) {

            printf("%d ", M[i][j]);
        }

        printf("\n");

    }

    printf("\nMatrix N:\n");

    for (int i = 0; i < m; i++) {

        for (int j = 0; j < n; j++) {

            printf("%d ", N[i][j]);

        }

        printf("\n");

    }

    printf("\nProduct Matrix:\n");

    for (int i = 0; i < l; i++) {

        for (int j = 0; j < n; j++) {

            printf("%d ", result[i][j]);
        }

        printf("\n");
     }

    for (int i = 0; i < l; i++) {

        free(M[i]);
        free(result[i]);
        
    }

    for (int i = 0; i < m; i++) {
        free(N[i]);

    }

    free(M);
    free(N);
    free(result);


    return 0;


}

