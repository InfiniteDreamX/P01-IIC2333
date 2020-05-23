#include "disk_utils.h"

#include <math.h>
#include <stdio.h>

#include "../cr_api/cr_API.h"
#include "common_utils.h"

unsigned int read_block_index(unsigned int block_number, uint8_t* buffer,
                              unsigned int start_index, unsigned int n_bytes) {
  FILE* file = fopen(disk_name, "rb");
  if (file == NULL) {
    exit_with_error("No se pudo abrir el archivo de la ruta '%s'", disk_name);
  }
  if (block_number >= MAX_BLOCKS) {
    exit_with_error("Numero de bloque fuera de rango: %d", block_number);
  }

  if (start_index + n_bytes > BLOCK_SIZE) {
    unsigned int start = start;
    unsigned int end = start_index + n_bytes;
    exit_with_error("Bytes del bloque fuera del rango: [%d, %d]", start, end);
  }

  unsigned int start_position = BLOCK_SIZE * block_number + start_index;

  fseek(file, start_position, SEEK_SET);
  unsigned int read = fread(buffer, 1, n_bytes, file);

  rewind(file);
  fclose(file);
  return read;
}

unsigned int read_block_partition_index(unsigned int partition,
                                        unsigned int block_number,
                                        uint8_t* buffer,
                                        unsigned int start_index,
                                        unsigned int n_bytes) {
  if (partition > 4 || partition < 1) {
    exit_with_error("Particion invalida para leer bloque: %d", partition);
  }
  unsigned int absolute_block = (partition - 1) * BLOCKS_PARTITION + block_number; 
  return read_block_index(absolute_block, buffer, start_index, n_bytes);
}

unsigned int read_block_partititon(unsigned int partition,
                                   unsigned int block_number, 
                                   uint8_t* buffer,
                                   unsigned int n_bytes){
  return read_block_partition_index(partition, block_number, buffer, 0, n_bytes);
}

unsigned int read_block(unsigned int block_number, 
                        uint8_t* buffer,
                        unsigned int n_bytes){
  return read_block_index(block_number, buffer, 0, n_bytes);
}
