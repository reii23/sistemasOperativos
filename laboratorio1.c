/*
consideraciones
- Se asume que el usuario ingresa un N mayor a la cantidad de hijos creados
- Se usa el tipo de dato unsigned long long ya que solo usando long, el calculo de los factoriales se desbordaba
- Por el tipo de dato utilizado se puede calcular hasta N = 20
1! + 2!+ 3!+ .. +20! = 2561327494111820313

algunos casos de prueba:
N = 20, RESULTADO = 2561327494111820313
N = 15, RESULTADO = 138879579704209680000
N = 10, RESULTADO = 4037913
N = 5, RESULTADO = 153
N = 3, RESULTADO = 9

*/

#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>


unsigned long long factorial(int n) {
    if(n == 0){
        return 1;
    } else{
        return (unsigned long long)n*factorial(n-1);
    }
}

int main(){
    int N; 
    int NUM_HIJOS;
    int i;

    // inputs
    printf("Ingrese n a calcular: ");
    scanf("%d", &N);
    printf("Ingrese la cantidad de hijos: ");
    scanf("%d", &NUM_HIJOS);

    // condicion de entrada para N y los hijos
    if (N< NUM_HIJOS){
        printf("El valor de N debe ser mayor o igual que la cantidad de los hijos");
        return (EXIT_FAILURE);
    }


    // se define el rango y resto de los hijos
    int rango = N / NUM_HIJOS;  
    int resto = N % NUM_HIJOS; 

    int pipefd[NUM_HIJOS][2];
    for(i=0; i < NUM_HIJOS; i++){
        if(pipe(pipefd[i]) == -1){
            perror("Error al crear el pipe");
            exit(EXIT_FAILURE);
        }
    }


    // creacion de los procesos hijos según el input entregado
    pid_t pid;
    for(i= 0; i < NUM_HIJOS; i++){
        pid = fork();

        if(pid == -1) {
            perror("Error al crear hijo");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            close(pipefd[i][0]);

            int primero = i*rango + 1; 
            int ultimo =  rango* (i + 1);

            if(i == NUM_HIJOS - 1){  
                ultimo = ultimo+resto; // resto al ultimo hijo
            }

            unsigned long long resultado = 0;

            // se calcula el factorial de cada uno de los hijos
            for(int j=primero; j<=ultimo; j++){
                resultado = resultado + factorial(j);
            }
            printf("Hijo: %d: Soy un proceso hijo (PID: %d) mi factorial es: %llu\n", i+1, getpid(), resultado);
            write(pipefd[i][1], &resultado, sizeof(unsigned long long));
            close(pipefd[i][1]);
            exit(EXIT_SUCCESS);
        }
    }


    // se llama a cada resultado de cada hijo y se suman para obtener el resultado total en el padre
    unsigned long long sumaTotal = 0;

    for(i = 0; i < NUM_HIJOS; i++){
        close(pipefd[i][1]); 
        unsigned long long sumaParcial = 0;

        read(pipefd[i][0], &sumaParcial, sizeof(unsigned long long));
        sumaTotal = sumaTotal + sumaParcial;
        close(pipefd[i][0]);
    }

   // se espera que terminen los hijos su ejecucion con un wait
    for (i=0; i < NUM_HIJOS; i++){
        wait(NULL);
    }
    printf("La suma de los factoriales de 1 a %d es: %llu\n", N, sumaTotal);
    return 0;
}