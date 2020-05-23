#pragma once

#include <stdint.h>


// ######### Utilidades para mostrar bytes ###########

// El siguiente grupo de funciones imprime un arreglo de bytes en distintas
// codificaciones. Para cada codificacion hay una funcion que imprime 
// cada byte codificado en una misma linea y otra funcion que recibe como
// argumento las columnas 'cols', la cual imprime cols bytes codificados por
// fila en el output.

// Ejemplo:
// uint8_t bytes = {0x23, 0x11, 0xB2, 0x00, 0xFF}.
// print_bytes_hex(bytes, 5);
//    -> 23  11  B2  00  FF
// print_bytes_hex_cols(bytes, 5, 2);
//    -> 23  11
//       B2  00
//       FF
// print_bytes_decimal(bytes, 5); 
//    -> 35  17  178  0  255
// print_bytes_decimal_cols(bytes, 5, 3);
//    -> 35  17  178
//       0   255

void print_bytes_binary(uint8_t* bytes, int length);

void print_bytes_binary_cols(uint8_t* bytes, int length, int cols);

void print_bytes_decimal(uint8_t* bytes, int length);

void print_bytes_decimal_cols(uint8_t* bytes, int length, int cols);

void print_bytes_hex(uint8_t* bytes, int length);

void print_bytes_hex_cols(uint8_t* bytes, int length, int cols);

void print_bytes_ascii(uint8_t* bytes, int length);

void print_bytes_ascii_cols(uint8_t* bytes, int length, int cols);

// ######### Fin utilidades Mostrar bytes ##########

// Establece un valor a un bit de un byte. Retorna un nuevo byte
uint8_t set_bit_to_byte(uint8_t byte, unsigned int position, unsigned int bit);

// Obtiene un 1 o 0 que representa el bit de la position 'position' de byte
unsigned int get_bit_from_byte(uint8_t byte, unsigned int position);
