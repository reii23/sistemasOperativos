/*
Consideraciones importantes:
- El define TAMANO_BUFFER indica el tamaño del buffer, si se quiere modificar, se debe cambiar su valor desde el programa
- El define CANT_PRODUCTORES indica la cantidad de productores, si se quiere modificar, se debe cambiar su valor desde el programa
- El define CANT_CONSUMIDORES indica la cantidad de consumidores, si se quiere modificar, se debe cambiar su valor desde el programa
- El define ELEMENTOS_A_GENERAR indica la cantidad de elementos totales a generar, si se quiere modificar, se debe cambiar su valor desde el programa

- Para verificar que un consumidor efectivamente consume el elemento, se imprime el cuadrado del elemento consumido. 
- El buffer se inicializa con el tamaño otorgado y rellenado con ceros, una posición con cero indica que no hay elementos ingresados en esa posición
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

// Definimos las constantes pedidas en el enunciado:
#define TAMANO_BUFFER 20 // tamaño del buffer
#define CANT_PRODUCTORES 10 // cantidad de productores
#define CANT_CONSUMIDORES 10 // cantidad de consumidores
#define ELEMENTOS_A_GENERAR 40 // cantidad de elementos a generar

// Definimos el buffer a utilizar con tamaño de TAMANO_BUFFER se incializa en 0 el inicio y fin
int buffer[TAMANO_BUFFER];
int inicio = 0; 
int fin = 0;

// Definimos los semaforos a utilizar
sem_t elementos;  // controlar la cant de elementos en el buffer
sem_t espacios; // controlar la cant de espacios en el buffer
sem_t mutex; // controla acceso al buffer
sem_t print_mutex; // controla la el print del buffer

// estructura que almacenara los resultados (cuadrados de los elementos que se han consumido)
int *resultados; 
int numResultados = 0;  // contador de resultados
int maxResultados = ELEMENTOS_A_GENERAR; // maximo de resultados que se pueden almacenar sera la cantidad de elementos a generar

// Funcion que imprime el estado del buffer luego de cada una de las operaciones producir o consumir
void printBuffer(){

    sem_wait(&print_mutex); // se bloquea el semaforo para que solo una hebra pueda imprimir

    printf("Estado del buffer:[");
    for(int i = 0; i < TAMANO_BUFFER; i++){
        printf("%2d", buffer[i]);
        if(i<TAMANO_BUFFER-1) printf(", ");
    }
    printf(" ]\n");

    sem_post(&print_mutex); // se libera el semaforo para que otra hebra pueda imprimir
}


// Funcion que imprime los resultados de los cuadrados de los elementos consumidos
void printResultados(){
    printf("Arreglo de resultados de los elementos consumidos al cuadrado: [");
    for(int i = 0; i<numResultados; i++){ // se recorre el arreglo de resultados
        printf(" %d", resultados[i]); // se muestra el resultado y se separa por comas mientras no sea el ultimo elemento
        if (i < numResultados-1) {
            printf(",");
        }

    }

    printf(" ]");
}



// Funcion productor: se encarga de producir un elemento con un numero entre 1 y 100 e insertar elemento en el buffer
void *productor(void *id){
    int productorId = *((int*) id);

    for(int i = 0; i < ELEMENTOS_A_GENERAR / CANT_PRODUCTORES; i++) {
        int elemento = rand() % 100 + 1;  // numero aleatorio entre 1 y 100

        sem_wait(&espacios); 
        sem_wait(&mutex);  // bloqueamos semaforo para ocupar la SC


        buffer[fin] = elemento; // insertamos el elemento en el buffer
        fin = (fin + 1) % TAMANO_BUFFER; // actualizamos el fin del buffer
        printf("Productor %d produce el elemento: %d\n", productorId, elemento);
        printBuffer(); // después de producir el elemento se muestra el buffer


        sem_post(&mutex); // liberamos el semaforo para que otra hebra pueda acceder a la SC
        sem_post(&elementos); // liberamos el semaforo para que los consumidores puedan consumir
    }


    return NULL;
}



// Funcion consumidor: se encarga de consumir un elemento del buffer, calcula el cuadrado del elemento consumido para verificar que fue consumido
void *consumidor(void *id){
    int consumidorId = *((int*) id);

    for(int i = 0; i < ELEMENTOS_A_GENERAR / CANT_CONSUMIDORES; i++){
        sem_wait(&elementos); // bloqueamos semaforo para ocupar la SC
        sem_wait(&mutex);

        int elemento = buffer[inicio]; // consumimos el elemento
        buffer[inicio] = 0; // se limpia el elemento consumido
        inicio = (inicio + 1) % TAMANO_BUFFER; // actualizamos el inicio del buffer
        int cuadrado = elemento * elemento; // almacenamos el cuadrado del elemento consumido

        if(numResultados < maxResultados){ // si el numero de resultados es menor al maximo de resultados se almacena el cuadrado
            resultados[numResultados++] = cuadrado;
        } else{                            // sino es porque ya no se pueden almacenar mas resultados
            printf("No se pueden ingresar mas resultados");
        }

        printf("Consumidor %d consume el elemento: %d , cuadrado: %d\n", consumidorId, elemento, cuadrado);
        printBuffer(); // despues de conusmir el elemento se muestra el estado del buffer

        sem_post(&mutex); // liberamos los semaforos para que otra hebra pueda acceder a la SC
        sem_post(&espacios); 
    }


    return NULL;
}

// MAIN
int main() {
    pthread_t productores[CANT_PRODUCTORES];  // id para hebras productores
    pthread_t consumidores[CANT_CONSUMIDORES];  // id para hebras consumidores

    int cantidadMaxima; // guardar la cant maxima de ids de cada hebra (productor o consumidor)
    if (CANT_PRODUCTORES>CANT_CONSUMIDORES){ // si la cant de productores es mayor a la de consumidores
        cantidadMaxima=CANT_PRODUCTORES;
    } else{
        cantidadMaxima=CANT_CONSUMIDORES; // sino, la cantidad maxima es la de los consumidores
    }

    int ids[cantidadMaxima];

     // inciializacion de los semaforos elementos, espacios, mutex y print
    sem_init(&elementos,0,0); // se inicializa en 0 ya que no hay elementos en el buffer
    sem_init(&espacios,0,TAMANO_BUFFER); // se inicializa en el tamaño del buffer ya que hay espacios disponibles
    sem_init(&mutex,0,1); // se inicializa en 1 ya que no hay hebras en la SC inicialmente
    sem_init(&print_mutex,0,1); // se inicializa en 1 ya que no hay hebras en la SC inicialmente

    resultados = (int *) malloc(maxResultados*sizeof(int)); // se reserva memoria para almacenar los resultados de los cuadrados segun el max de resultados


     // inicialización del buffer con ceros (cero indica que no hay elementos)
    for(int i=0; i < TAMANO_BUFFER; i++){
        buffer[i]=0;
    }

    // se crean las hebras productores, se les asigna un id y se ejecuta la funcion productor
    for(int i=0; i < CANT_PRODUCTORES; i++){
        ids[i] = i;
        pthread_create(&productores[i], NULL, productor, &ids[i]);
    }

    // se crean las hebras consumidores se les asigna un id y se ejecuta la funcion consumidor
    for(int i=0; i < CANT_CONSUMIDORES; i++){
        ids[i] = i;
        pthread_create(&consumidores[i], NULL, consumidor, &ids[i]);
    }

    // se espera con join a que finalicen los productores
    for(int i=0; i < CANT_PRODUCTORES; i++){
        pthread_join(productores[i], NULL);
    }

    // se espera con join a que finalicen los consumidores
    for(int i = 0; i < CANT_CONSUMIDORES; i++){
        pthread_join(consumidores[i], NULL);
    }

    // se muestran ademas, el array con los resultados de los elementos consumidos
    printResultados();

    return 0;
}