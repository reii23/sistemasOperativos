# Laboratorios de Sistemas Operativos 1/2024 + Otros programas

- Autor: Reinaldo Pacheco Parra
- Sección: C-3 13212-0 
- Versión: gcc 11.4.0

- Laboratorio 1: Cálculo de Suma de Factoriales en Paralelo usando fork
Consiste en un programa que pregunta al usuario una cantidad N a calcular, la cual será el factorial del número, y una cantidad de hijos. El programa generará procesos hijos según el número ingresado por el usuario. Cada hijo calculará una porción del factorial y luego se suma la cantidad calculada por cada uno de los hijos para obtener el factorial total del número. Por pantalla se muestra el factorial calculado por cada uno de los hijos y el factorial total calculado.

- Laboratorio 2: Contador de palabras en un archivo usando pipes y fork
Consiste en un programa que recibe un archivo.txt y una cantidad de hijos. El archivo de texto será dividido según la cantidad de hijos ingresados otorgando un sector del archivo a cada hijo. Cada uno de los hijos creados se encargará de contar la cantidad de palabras encontradas en su sector y enviará al padre la cantidad obtenida a través de pipes. El padre sumará la cantidad total de cada una de las palabras a través de un contador para cada uno. Por pantalla se muestra la cantidad de palabras encontradas por cada sector e hijo y luego la cantidad de palabras totales encontradas.

- Laboratorio 3: Contador de eventos en un archivo usando hebras (threads)
Consiste en un programa que recibe un archivo registros.txt que contiene lineas de registros y palabras clave que lo identifican como: "Error", "Advertencia", etc. El programa realiza la lectura del archivo y cuenta la cantidad de eventos encontrados a través del uso de hebras, las cuales se encuentran definidas en el archivo pero pueden ser modificadas por el usuario. Cada hebra revisa un sector correspondiente del archivo y envían el resultado a una variable global que contiene la cuenta de cada evento. Además, para que el resultado sea correcto y no se vea modificado por la ejecución de las hebras, se utilizan semáforos mutex los que controlan a través de lock y unlock que solo una hebra a la vez modifique valores globales.

- Laboratorio 4: Problema del Productor y Consumidor
Consiste en un programa que simula el problema del productor y consumidor a través del uso de hebras, para ello se establece el tamaño del buffer a utilizar, una cantidad de productores, una cantidad de consumidores y una cantidad de elementos a generar. Los productores generan elementos y los ingresan en el buffer, mientras que los consumidores quitan el elemento del buffer incial y los ingresan a otro buffer que contiene los elementos consumidos. Por pantalla se muestra el estado del buffer después de cada una de las operaciones (ingresar o consumir un elemento), la hebra que ha realizado la operación y finalmente el buffer con los elementos totales consumidos.

### Los laboratorios fueron realizados y ejecutados en Linux, Ubuntu 22.04 usando C versión gcc 11.4.0

### Contacto
Ante cualquier duda o problema contactarme a través del correo reinaldo.pacheco@usach.cl
