// sumatoria para un N

#include <stdio.h>
#include <pthread.h>

#define NUM_THREADS 2
#define N 10

int array[N] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
int partial_sum[NUM_THREADS];
pthread_t threads[NUM_THREADS];

// Función que calculará la sumatoria parcial
void *calcular_sumatoria(void *arg) {
    int thread_id = *(int *)arg; // recasteo de un argumento de un puntero void a un puntero int
    int elements_per_thread = N / NUM_THREADS; // balance carga
    int start = thread_id * elements_per_thread;
    int end = start + elements_per_thread;
    int sum = 0;

    for (int i = start; i < end; i++) {
        sum += array[i];
    }

    partial_sum[thread_id] = sum; // potencial problema de concurrencia, ya que se está accediendo a la misma variable desde diferentes hebras
    pthread_exit(NULL);
}

int main() {
    for (int i = 0; i < NUM_THREADS; i++) {
        int *thread_id = (int *)malloc(sizeof(int)); // cantidad de hebras a crear
        *thread_id = i;

        if (pthread_create(&threads[i], NULL, calcular_sumatoria, thread_id)) { //en base al thread_id voy a saber el limite donde tiene que trabajar cada hebra
            fprintf(stderr, "Error al crear la hebra %d\n", i);
            return 1;
        }
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL); // estructura de pthread_join: parametro y valor de retorno
    }

    int total_sum = 0;
    for (int i = 0; i < NUM_THREADS; i++) {
        total_sum += partial_sum[i]; // se suman todas las sumas parciales para obtener la sumatoria total
    }

    printf("Sumatoria final: %d\n", total_sum);

    return 0;
}

