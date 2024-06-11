// mutex: se usa para proteger una sección crítica.


#include <stdio.h>
#include <pthread.h>

#define NUM_THREADS 5
#define ITERATIONS 10000

int contador_compartido = 0; // variable global (recurso crítico)
pthread_mutex_t mutex_contador; // mutex es una variable global

void* tarea_hebra(void* arg) {
    for (int i = 0; i < ITERATIONS; ++i) {
        // Bloquear el mutex antes de acceder al contador compartido
        // pthread_mutex_lock(&mutex_contador);

        // Sección crítica: incrementar el contador compartido
        contador_compartido++;

        // Desbloquear el mutex después de actualizar el contador
        // pthread_mutex_unlock(&mutex_contador);
    }

    // pthread_mutex_lock = wait
    // pthread_mutex_unlock = signal

    // si no lo protegemos, tenemos un problema de condición de carrera (hebras se topan)

    pthread_exit(NULL);
}

int main() {
    pthread_t hebras[NUM_THREADS];

    // Inicializar el mutex
    // estructura de pthread_mutex_init: dirección de memoria y atributos, en NULL inicia con atributos por defecto
    if (pthread_mutex_init(&mutex_contador, NULL) != 0) { 
        fprintf(stderr, "Error al inicializar el mutex\n");
        return 1;
    }

    // Crear hebras
    for (int i = 0; i < NUM_THREADS; ++i) {
        if (pthread_create(&hebras[i], NULL, tarea_hebra, NULL) != 0) {
            fprintf(stderr, "Error al crear la hebra %d\n", i);
            return 1;
        }
    }

    // Esperar a que todas las hebras terminen
    for (int i = 0; i < NUM_THREADS; ++i) {
        pthread_join(hebras[i], NULL);
    }

    // Destruir el mutex después de su uso
    pthread_mutex_destroy(&mutex_contador);

    // Imprimir el resultado final del contador
    printf("Valor final del contador compartido: %d\n", contador_compartido);

    return 0;
}

