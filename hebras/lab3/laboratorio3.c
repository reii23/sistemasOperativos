
// instrucciones de ejecución y otros en el readme.md

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define MAXLARGO 256
#define CANTHEBRAS 100 // modificar para cambiar la cant hebras

char **lineas;
int cantLineas;
const char *palabraClave;

pthread_mutex_t mutex;


int cont; // variable global que cuente las veces que se encuentra la palabra clave
int idHebras[CANTHEBRAS]; 
int palabrasEncontradas[CANTHEBRAS];


void *buscarPalabraClave(void *arg){
    int idHebra = *(int *)arg;
    int lineasPorHebra = cantLineas/CANTHEBRAS;
    int inicio = idHebra*lineasPorHebra;
    int fin;

    if (idHebra == CANTHEBRAS-1){
        fin = cantLineas;
    }else{
        fin = inicio+lineasPorHebra;
    }

    printf("La hebra %d revisa desde la linea %d hasta la linea %d\n",idHebra,inicio+1,fin);

    for(int i = inicio; i < fin; i++){
        if (strstr(lineas[i], palabraClave)!= NULL) {
            // wait para proteger la seccion critica
            pthread_mutex_lock(&mutex);
            printf("La hebra %d encontró en la linea %d: %s" ,idHebra,i+1,lineas[i]);
            cont++;
            palabrasEncontradas[idHebra]++;
            pthread_mutex_unlock(&mutex);
            // signal para liberar la seccion critica
        }
    }

    return NULL;
}

int main(int argc,char *argv[]){ // nombreArchivoCompilado, nombre.txt, palabraClave
    if(argc != 3) {
        return 1;
    }


    const char  *nombreArchivo=argv[1]; // posicion del nombre
    palabraClave=argv[2]; // posicion de la palabra a buscar
    FILE *archivo = fopen(nombreArchivo,"r");
    if(archivo == NULL){
        perror("Error al abrir archivo");
        return 1;
    }


    lineas = malloc(sizeof(char *)*1000); // asignado de memoria para la cant lineas
    if(lineas == NULL) {
        perror("Error de asignar memoria "); 
        fclose(archivo);
        return 1;
    }

    cantLineas = 0;
    char buffer[MAXLARGO];


    while (fgets(buffer, MAXLARGO, archivo) != NULL){
        lineas[cantLineas] = malloc(strlen(buffer)+1);
        if(lineas[cantLineas] == NULL){
            perror("Error de asignar memoria");

            fclose(archivo);
            return 1;
        }

        strcpy(lineas[cantLineas],buffer);
        cantLineas++;
    }
    fclose(archivo);

    pthread_t threads[CANTHEBRAS]; 
    pthread_mutex_init(&mutex, NULL);
    cont = 0;

    for (int i = 0; i < CANTHEBRAS; i++){
        idHebras[i] = i;
        palabrasEncontradas[i] = 0;

        int error = pthread_create(&threads[i], NULL, buscarPalabraClave, &idHebras[i]);
        if (error != 0){
            fprintf(stderr, "Error create hebra %d: %s\n", i, strerror(error));
            return 1;
        }
    }

    for (int i = 0; i < CANTHEBRAS; i++){
        int error = pthread_join(threads[i], NULL);
        if (error != 0){
            fprintf(stderr, "Error join de hebra %d: %s\n", i, strerror(error));
            return 1;
        }
    }

    for (int i = 0; i < cantLineas; i++){
        free(lineas[i]);
    }
    free(lineas);

    // se realiza un destroy mutex para liberar recursos de memoria
    pthread_mutex_destroy(&mutex);

    for (int i = 0; i < CANTHEBRAS; i++){
        // print de la cantidad de palabras por cada hebra
        printf("La hebra %d encontró %d veces la palabra clave\n", idHebras[i],palabrasEncontradas[i]);
    }

    // print de las palabras totales encontradas
    printf("Palabras clave encontradas en total: %d\n", cont);  
    return 0;
}
