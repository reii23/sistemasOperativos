/*P4. PEP 1 - Sistemas Operativos Vespertino 1.2024 

Analiza el sgte código en C y responde las sgtes preguntas:
a) describe el árbol de procesos resultante después de ejecutar el programa, incluyendo el proceso raíz, los hijos y los nietos, 
indicando claramente la relación entre cada proceso (padre/hijo)
b) después de ejecutar el programa describe la salida exacta en la consola, incluyendo todos los mensajes impresos por los procesos
con sus respectivos IDs(PID) y IDs de los procesos padres(PPID). Considera que el proceso principal tiene PID 100 y se van
creando procesos con PID 200, 300, 400 y así
*/


# include <stdio.h>
# include <unistd.h>
# include <sys/wait.h>

int main() {
    printf("Proceso Raiz (PID: %d)\n", getpid());

    for (int i = 0; i < 2; i++) {
        pid_t pid = fork();
        if (pid == 0) { // Usar pid en lugar de child_pid
            printf("Proceso hijo %d (PID: %d, PPID: %d)\n", i + 1, getpid(), getppid());

            int num_grandchildren = (i % 2 == 0) ? 2 : 1;
            for (int j = 0; j < num_grandchildren; j++) {
                pid_t grandchild_pid = fork();
                if (grandchild_pid == 0) {
                    printf("Nieto de proceso hijo %d (PID: %d, PPID: %d)\n", i + 1, getpid(), getppid());
                    return 0;
                }
                // Este loop debe estar fuera del anterior
            }
            for (int j = 0; j < num_grandchildren; j++) {
                wait(NULL);
            }
            return 0; // Asegura que los hijos no crean más hijos
        }
        // Este loop espera por ambos hijos
    }
    for (int i = 0; i < 2; i++) {
        wait(NULL);
    }
    return 0;
}
