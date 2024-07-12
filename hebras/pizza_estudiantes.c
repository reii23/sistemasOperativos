/* 
Seis estudiantes están sentados alrededor de una mesa redonda con una pizza gigante en el medio.
Los estudiantes comen pizza usando tenedores y cuchillos, en donde cada estudiante para comer usa un tenedor y un
cuchillo. En la mesa hay tres tenedores y tres cuchillos. Cada estudiante usa el siguiente algoritmo para comer:
• Tomar un cuchillo
• Tomar un tenedor
• Cortar un pedazo de pizza y comer
• Devolver el cuchillo y tenedor a su posición original

(b) Implemente el método Cenar() el cual espera a que los estudiantes tengan un tenedor y un cuchillo antes
de comer. Luego este método debe llamar a la función Comer(), y luego los estudiantes deben devolver
los cubiertos a su posici´on original. Este método sólo debe hacer uso de semáforos contadores.
*/

#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define NUMERO_ESTUDIANTES 6

sem_t tenedores;
sem_t cuchillos;

void Comer(int id){
    printf("El estudiante %d esta comiendo\n", id);
    sleep(1); // Comiendo
}

void* Cenar(void* arg) {
    int id = *(int*)arg;


    // Wait para tomar tenedor y cuchillo
    sem_wait(&tenedores);
    printf("El estudiante %d tomo un tenedor\n", id);
    sem_wait(&cuchillos);
    printf("El estudiante %d tomo un cuchillo\n", id);

    Comer(id); // se llama a la funcion comer
    
    // Signal para devolver tenedor y cuchillo
    sem_post(&tenedores);
    printf("El estudiante %d devolvio el tenedor\n", id);
    sem_post(&cuchillos);
    printf("El estudiante %d devolvio el cuchillo\n", id);

    return NULL;
}

int main(){
    pthread_t estudiantes[NUMERO_ESTUDIANTES];
    int ids[NUMERO_ESTUDIANTES];

    // se inicializan los 3 tenedores y 3 cuchillos posibles en la mesa
    sem_init(&tenedores, 0, 3); 
    sem_init(&cuchillos, 0, 3);

    for(int i=0; i<NUMERO_ESTUDIANTES; i++){
        ids[i] = i;
        pthread_create(&estudiantes[i], NULL, Cenar, &ids[i]);
    }

    for(int i=0; i <NUMERO_ESTUDIANTES; i++){
        pthread_join(estudiantes[i], NULL);
    }

    return 0;
}