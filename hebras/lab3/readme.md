# Laboratorio 3 - Sistemas Operativos Vespertino

## Consideraciones

Para ejecutar el programa, se debe ingresar por consola la siguiente estructura:

./nombreArchivoCompilado <nombreArchivoaLeer> <palabraClave>

Ejemplo: 

gcc laboratorio3.c -o laboratorio
./laboratorio registros.txt Advertencia

Si se quiere cambiar la cantidad de hebras a utilizar, se debe modificar el valor #define de CANTHEBRAS

Resultados para registros .txt de 200 lineas:
- Advertencia = 28
- Evento = 80
- Información = 50
- Error = 42
