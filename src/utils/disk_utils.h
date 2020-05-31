#pragma once

#include <stdint.h>

// Constantes de utilidad
static const int MAX_SIZE = 2147483648;       // bytes del disco completo
static const int PARTITION_SIZE = 536870912;  // bytes de particion
static const int BLOCK_SIZE = 8192;           // bytes de un block
static const int BLOCKS_PARTITION = 65536;    // bloques es una particion
static const int MAX_BLOCKS = 262144;         // bloques en todo el disco

// Ejemplo para mostrar el bloque directorio de la tercera particion:
//      #include "../cr_api/cr_API.h"
//      #include "../utils/disk_utils.h"
//      cr_mount("simdiskfilled.bin");
//      uint8_t bytes[BLOCK_SIZE];
//      read_block_partititon(1, 0, bytes, BLOCK_SIZE); -> primer bloque es directorio!
//      print_bytes_ascii_cols(bytes, BLOCK_SIZE, 40);  -> Veremos nombres de archivos
//      print_bytes_hex_cols(bytes, BLOCK_SIZE, 30); -> Vemos hexadecimal     

// Carga en el buffer los primeros 'n_bytes' bytes del bloque
// de número 'block_number'. Retorna la cantidad de bytes que efectivamente
// se ecribieron en el buffer
unsigned int read_block(unsigned int block_number, uint8_t* buffer,
                        unsigned int n_bytes);

// Lo mismo que read_block, pero se especifica la particion (1 al 4)
// y block_number es relativo al inicio de la particion.
// Ejemplo: para leer 100 bytes del bloque directorio de la segunda particion:
// uint8_t buff[100]; -> buffer en stack!
// read_block_partition(2, 0, buff, 100);
unsigned int read_block_partititon(unsigned int partition,
                                   unsigned int block_number, uint8_t* buffer,
                                   unsigned int n_bytes);

// Carga en el buffer'n_bytes' bytes del bloque
// de número 'block_number', empezando desde el byte 'start_index'.
// Ejemplo: Leer del 3er al 7mo byte del 200avo bloque.
// uint8_t buff[4]; -> buffer en stack!
// read_block_index(199, buff, 2, 4);
unsigned int read_block_index(unsigned int block_number, uint8_t* buffer,
                              unsigned int start_index, unsigned int n_bytes);

// Analogo a read_block_partition pero con index
unsigned int read_block_partition_index(unsigned int partition,
                                        unsigned int block_number,
                                        uint8_t* buffer,
                                        unsigned int start_index,
                                        unsigned int n_bytes);

// Retorna direccion del primer bloque vacio de la particion partition
unsigned int get_empty_block_direction(unsigned int partition);