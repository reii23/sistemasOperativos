/*
por el momento solo llega a calcular con N = 20
1!+2!+3!+ ...+20! = 2561327494111820313

consideraciones:
- Se asume que el usuario ingresa un N mayor a la cantidad de hijos creados
- Se usa el tipo de dato unsigned long long ya que solo usando long, el calculo de los factoriales se desbordaba
*/

#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>


unsigned long long factorial(int n) {
    if(n == 0){
        return 1;
    } else{
        return (unsigned long long)n * factorial(n-1);
    }
}

int main(){
    int N;
    int NUMERO_HIJOS;
    int i;

    printf("Ingrese n a calcular: ");
    scanf("%d", &N);
    printf("Ingrese la cantidad de hijos: ");
    scanf("%d", &NUMERO_HIJOS);

    if (N< NUMERO_HIJOS){
        printf("El valor de N debe ser mayor o igual que la cantidad de hijos");
        return (EXIT_FAILURE);
    }

    int rango = N / NUMERO_HIJOS;  
    int resto = N % NUMERO_HIJOS; 

    int pipefd[NUMERO_HIJOS][2];
    for(i = 0; i < NUMERO_HIJOS; i++){
        if(pipe(pipefd[i]) == -1){
            perror("Error al crear el pipe");
            exit(EXIT_FAILURE);
        }
    }

    // procesos hijos
    pid_t pid;
    for(i = 0; i < NUMERO_HIJOS; i++){
        pid = fork();

        if(pid == -1) {
            perror("Error al crear hijo");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            close(pipefd[i][0]);
            int primero = i * rango + 1; 
            int ultimo =  rango* (i + 1);

            if(i == NUMERO_HIJOS - 1){  
                ultimo = ultimo + resto;
            }
    
            unsigned long long resultado = 0;

            for(int j = primero; j <= ultimo; j++){
                resultado = resultado + factorial(j);
            }
            printf("Hijo: %d: Soy un proceso hijo (PID: %d),mi factorial es: %llu\n", i+1, getpid(), resultado);
            write(pipefd[i][1], &resultado, sizeof(unsigned long long));
            close(pipefd[i][1]);
            exit(EXIT_SUCCESS);
        }
    }

    // suma de factoriales
    unsigned long long sumaTotal = 0;
    for(i = 0; i < NUMERO_HIJOS; i++){
        close(pipefd[i][1]); 
        unsigned long long sumaParcial = 0;
        read(pipefd[i][0], &sumaParcial, sizeof(unsigned long long));
        sumaTotal = sumaTotal + sumaParcial;
        close(pipefd[i][0]);
    }

    for (i = 0; i < NUMERO_HIJOS; i++){
        wait(NULL);
    }
    printf("La suma de los factoriales de 1 a %d es: %llu\n", N, sumaTotal);
    return 0;
}