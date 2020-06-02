# P01-IIC2333

# Integrantes
Rafael Fernández Sánchez 17639123

Nicolás Vargas Cisternas 15634795

Jorge Pérez Facuse 17637619

Sergio Gazali 17636655

# Supuestos y decisiones

La primera decisión tomada, y que define el comportamiento del las funciones de la API, fue crear una serie de utilidades de bajo nivel, para poder abstraernos del acceso mismo al disco en la programación, y usar lógica de nivel más alto. Aumentando también el orden en el código. 

El resultado de esto son los módulos disk_utils, byte_utils, y common utils. disk_utils provee funciones para la lectura y escritura de información en el disco, esto se hace ingresando parámetros como número de bloque (absoluto o relativo a la partición), número de byte, partición, entre otros. Además de una función para obtener la dirección del primer bloque vacío del disco. byte_utils provee funciones para manejo de bytes, es decir, imprimirlos en distintas interpretaciones y formatos, obtener un bit de un byte, o setear el valor de un bit en un byte. Por último, common_utils provee funciones para invertir arreglos de bytes, y para manejar errores. Vale la pena comentar con esto, que con la función exit_with_error se manejan todos los errores, llamándola con un mensaje específico para cada error.

Otra decisión importante de diseño fue crear un main.c interactivo para probar más cómodamente nuestras funciones. Con el, se puede ingresar cada función que deseemos utilizar por consola.

Por último, la escritura al disco escribe directamente a él (no se espera a que se llame close para actualizar su información). Esto se permite de manera eficiente por haber hecho los disk_utils, con los que acceder al disco es suficientemente cómodo para que se haga en el momento, y no se deje toda la actualización de la información para el final.