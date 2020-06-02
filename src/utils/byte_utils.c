#include "byte_utils.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common_utils.h"

int SPACING = 0;

// Source:
// https://stackoverflow.com/questions/1024389/print-an-int-in-binary-representation-using-c
char* byte2bin(uint8_t a, char* buffer, int buf_size) {
  buffer += (buf_size - 1);
  for (int i = 7; i >= 0; i--) {
    *buffer-- = (a & 1) + '0';
    a >>= 1;
  }
  return buffer;
}
// Funcion privada (sin header) que comparten todos los prints.
void base_print(uint8_t* bytes, int length, char* format_type, int col_width,
                int cols_per_line) {

  if (cols_per_line <= 0) {
    exit_with_error("Las columnas ingresadas: %d, no son validas.", cols_per_line);
  }

  if (length <= 0){
    exit_with_error("El largo pasado: %d, no es valido para imprimir", length);
  }

  int width_length = (int)(ceil(log10(col_width)));
  int format_len = strlen(format_type);
  int tail_length = width_length + format_len;

  char length_and_type[(tail_length + 1) * sizeof(char)];
  sprintf(length_and_type, "%d%s", col_width, format_type);

  char format[(tail_length + 4) * sizeof(char)];
  strcpy(format, "%-");
  strcat(format, length_and_type);

  for (int i = 0; i < length; i++) {
    uint8_t byte = bytes[i];

    if (strcmp(format_type, "b") == 0) {
      // Transformacion a binario
      char buffer[9];
      buffer[8] = '\0';
      byte2bin(byte, buffer, 8);
      printf("%s", buffer);
    } else {
      // Imprime usando el formato generado para que se alinien las columnas
      printf(format, byte);
    }
    // Nueva linea cuando se llega al limite de columnas
    if ((i + 1) % cols_per_line == 0) {
      printf("\n");
    } else {
      for (int space_i = 0; space_i < SPACING; space_i++) {
        printf(" ");
      }
    }
  }
}

void print_bytes_binary_cols(uint8_t* bytes, int length, int cols) {
  int col_width = 8;
  base_print(bytes, length, "b", col_width, cols);
}

void print_bytes_binary(uint8_t* bytes, int length) {
  print_bytes_binary_cols(bytes, length, length);
}

void print_bytes_decimal_cols(uint8_t* bytes, int length, int cols) {
  // Caracteres maximos de un numero decimal de 1 byte -> 255 -> 3
  int col_width = 3;
  base_print(bytes, length, "d", col_width, cols);
}

void print_bytes_decimal(uint8_t* bytes, int length) {
  // Imprime en una fila de largo length
  print_bytes_decimal_cols(bytes, length, length);
}

void print_bytes_hex_cols(uint8_t* bytes, int length, int cols) {
  char* format = ".2X";
  int col_width = 3;
  base_print(bytes, length, format, col_width, cols);
}

void print_bytes_hex(uint8_t* bytes, int length) {
  print_bytes_hex_cols(bytes, length, length);
}

void print_bytes_ascii(uint8_t* bytes, int length) {
  print_bytes_ascii_cols(bytes, length, length);
}

void print_bytes_ascii_cols(uint8_t* bytes, int length, int cols) {
  int col_width = 1;
  base_print(bytes, length, "c", col_width, cols);
}

// Source:
// https://stackoverflow.com/questions/47981/how-do-you-set-clear-and-toggle-a-single-bit
uint8_t set_bit_to_byte(uint8_t byte, unsigned int position, unsigned int bit){
  if (position > 7 || position < 0){
    exit_with_error("Rango invalido para posicion de bit: %d", position);
  }

  if (bit > 1 || bit < 0){
    exit_with_error("Rango invalido para bit: %d", bit);
  }
  
  if (bit){
    byte |= 1 << position;
  } else {
    byte &= ~(1 << position);
  }
  return byte;
}

unsigned int get_bit_from_byte(uint8_t byte, unsigned int position){
  if (position > 7 || position < 0) {
    exit_with_error("Rango invalido para posicion de bit: %d", position);
  }
  return (byte >> position) & 1;
}