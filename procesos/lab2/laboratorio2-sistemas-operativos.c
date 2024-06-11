/*
consideraciones:

- Al momento de dividir el archivo en las secciones corrrespondientes a cada hijo
puede que algunas palabras específicas se cuenten más de una vez si es que la división se realiza en medio de esa palabra,
esta consideración se hizo con el fin de evitar que una palabra se cortara a la mitad y se contara como una palabra diferente.
este problema se da solo en algunas palabras.

- En cuanto a la división de las secciones, se dividen de manera equitativa (aproximadamente) indicando el rango con el inicio
y el final que debe revisar cada hijo

- En cuanto a la lógica de los pipes y forks, funciona correctamente, ya que las palabras contadas por cada hijo y enviadas
al padre, son contadas y sumadas correctamente por el padre.

- Como los procesos no terminan en el mismo orden en que se ejecutan, puede que en algunas ejecuciones los hijos se muestren
entre medio de algun conteo de palabras en el print.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <ctype.h>

#define BUFFER_SIZE 1024
#define MAXPALABRA 100
#define MAXPALABRALEN 50

// función encargada de pasar a minuscula las palabras para que todas tengan el mismo formato
void pasarMinuscula(char *str) {
    for (int i=0; str[i]; i++) {
        str[i]=tolower(str[i]);
    }
}


// funcion encargada de dividir el archivo en secciones para cada hijo a crear
int dividirSecciones(FILE *fp, int posActual, int posSiguiente){
    fseek(fp, posActual, SEEK_SET);
    if(posActual != 0 && posSiguiente==-1) {
        while(posActual> 0 && !isspace(fgetc(fp))) {
            fseek(fp, --posActual,SEEK_SET);
        }
    }
    else if(posActual != 0 && posSiguiente  == 1){
        char c;
        while ((c=fgetc(fp)) != EOF && !isspace(c));
        posActual = ftell(fp);
    }
    return posActual;
}

// función que cuenta las palabras de cada sección del archivo
void contarSeccion(FILE *fp, int inicio, int fin, int pipeWriteFin){
    fseek(fp, inicio, SEEK_SET);
    char buffer[BUFFER_SIZE];
    char words[MAXPALABRA][MAXPALABRALEN]={0};
    int frecuenciaA[MAXPALABRA]={0};
    int cantPalabras= 0;


    while (ftell(fp)<fin && fgets(buffer, sizeof(buffer), fp)){
        pasarMinuscula(buffer);
        char *token = strtok(buffer, " ,.-!?\n\t");
        while (token != NULL) {

            // contador para identificar cuando una palabra es encontrada
            int palabraEncontrada = 0;
            for (int i=0; i  < cantPalabras; i++){
                if (strcmp(words[i], token) == 0){
                    frecuenciaA[i]++;
                    palabraEncontrada=1;
                    break;
                }

            }

            
            if (!palabraEncontrada && cantPalabras < MAXPALABRA){
                strcpy(words[cantPalabras],token);
                frecuenciaA[cantPalabras++] = 1;
            }
            token = strtok(NULL, " ,.-!?\n\t");
        }
    }

    write(pipeWriteFin, &cantPalabras, sizeof(cantPalabras));
    for (int i = 0; i<cantPalabras; i++) {
        write(pipeWriteFin,words[i],sizeof(words[i]));
        write(pipeWriteFin,&frecuenciaA[i],sizeof(frecuenciaA[i]));
    }
}

// lectura de archivo, recibe parámetros de cnt de hijos
int main() {
    int NUMHIJOS;
    char NOMBRE_ARCHIVO[100];
    printf("Ingresa el nombre del archivo a leer: ");
    scanf("%s", NOMBRE_ARCHIVO);

    FILE *fp = fopen(NOMBRE_ARCHIVO, "r");
    if (!fp) {
        perror("Error al abrir el archivo");
        return EXIT_FAILURE;
    }


    printf("Ingresa cantidad de hijos:");
    scanf("%d",&NUMHIJOS);
    fseek(fp, 0,SEEK_END);
    int sizeArchivo= ftell(fp);

    int pipes[NUMHIJOS][2];
    pid_t pids[NUMHIJOS];


// division de las secciones segun el peso del archivo en la cantidad de hijos
    int ultimaSeccion = 0;
    for (int i = 0; i < NUMHIJOS; i++){
        int inicio = (sizeArchivo/NUMHIJOS)*i;
        if (i > 0) {
            inicio = dividirSecciones(fp, inicio, -1);
        }
        if (inicio < ultimaSeccion) {
            inicio = ultimaSeccion;
        }

        int fin;
        if (i == NUMHIJOS - 1) {
            fin = sizeArchivo;
        } else {
            fin = (sizeArchivo / NUMHIJOS) * (i + 1);
            fin = dividirSecciones(fp, fin, 1);
        }
        ultimaSeccion = fin;


        pipe(pipes[i]);
        pids[i] = fork();

    if (pids[i] == 0) {
        fclose(fp);
        fp = fopen(NOMBRE_ARCHIVO, "r");
        close(pipes[i][0]);
        contarSeccion(fp, inicio, fin, pipes[i][1]);
        // se printean las secciones que debe revisar cada hijo
        printf("Hijo %d (revisa desde %d hasta %d)\n", getpid(), inicio, fin);
        fclose(fp);
        close(pipes[i][1]);
        exit(0);
    }
}
    
    char palabras[MAXPALABRA][MAXPALABRALEN] = {0};
    int frecuenciaPalabras[MAXPALABRA] = {0};
    int contadorPalabras = 0;

        for (int i = 0; i < NUMHIJOS; i++){
        waitpid(pids[i], NULL, 0);    
        close(pipes[i][1]);

        int cantPalabrasPipe;
        read(pipes[i][0], &cantPalabrasPipe,sizeof(cantPalabrasPipe));
        char word[MAXPALABRALEN];
        int frecuencia;

    // se printean las palabras encontradas por cada hijo
        printf("Hijo %d encontró las siguientes palabras:\n", pids[i]);
        for (int j = 0; j < cantPalabrasPipe; j++) {
            read(pipes[i][0], word, sizeof(word)); 
            read(pipes[i][0], &frecuencia, sizeof(frecuencia));

            printf("%s: %d\n", word, frecuencia); 

            int palabraEncontrada = 0;
            for (int k = 0; k < contadorPalabras; k++) {
                if (strcmp(palabras[k], word) == 0) {
                    frecuenciaPalabras[k] = frecuenciaPalabras[k] + frecuencia;
                    palabraEncontrada = 1;
                    break;
                }
            }
            if (!palabraEncontrada && contadorPalabras < MAXPALABRA){
                strcpy(palabras[contadorPalabras], word);
                frecuenciaPalabras[contadorPalabras++] = frecuencia;
            }
        }
        close(pipes[i][0]); 
    }


    // se suma la frecuencia de todas las encontradas por los hijos para mostrarlas por el padre:
    printf("Padre: Frecuencia de todas las palabras:\n");
    for (int i = 0; i < contadorPalabras; i++) {
        printf("%s: %d\n", palabras[i], frecuenciaPalabras[i]);
    }

    // funcion para buscar una palabra especifica en el texto
    char buscarPalabra[MAXPALABRALEN];
    printf("Ingresa una palabra para buscar su frecuencia escriba 'termina' para cerrar el programa: ");
    scanf("%s", buscarPalabra);
    while (strcmp(buscarPalabra, "termina") != 0) {
        int palabraEncontrada = 0;


        for (int i = 0; i < contadorPalabras; i++) {
            if (strcmp(palabras[i], buscarPalabra) == 0) {
                printf("La palabra '%s' se encuentra %d veces en el texto\n", buscarPalabra, frecuenciaPalabras[i]);
                palabraEncontrada = 1;
                break;
            }
        }
        if (!palabraEncontrada) {
            printf("La palabra '%s' no se encuentra en el texto\n", buscarPalabra);
        }
        printf("Ingresa una palabra para buscar su frecuencia escriba 'termina' para cerrar el programa): ");
        scanf("%s", buscarPalabra);
    }

    fclose(fp);
    return 0;
}
