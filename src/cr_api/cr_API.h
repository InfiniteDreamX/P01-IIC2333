#pragma once
#include <stdlib.h>

typedef struct cr_file crFILE;
struct cr_file
{
   unsigned int partition; // particion donde se encuentra el archivo
   unsigned int block; // bloque de datos actual.
   // IMPORTANTE: en 'r', block es el indice con el que obtenemos un puntero en el array data_blocks
   // en 'w', block es directamente un puntero, al bloque de datos al que se está escribiendo.
   unsigned int byte;  // indice dentro del bloque de datos actual
   int block_number;
   char mode; // r o w
   unsigned int references; // 4 bytes para cantidad de hardlinks
   unsigned long size; // 8 bytes para tamaño de archivo
   unsigned int data_blocks[]; // Array con las direcciones de los bloques de datos del archivo
};
char* disk_name;
void cr_mount(char* diskname);

void cr_ls(unsigned disk);

crFILE* cr_open(unsigned disk, char* filename, char mode);

int cr_exists(unsigned disk, char* filename);

crFILE* cr_open(unsigned disk, char* filename, char mode);

int cr_read(crFILE* file_desc, void* buffer, int nbytes);

/* El funcionamiento de esta funcion sera el siguiente:
   - Si recibe un orig, entonces copia ese archivo a la ruta de desc. En ese caso, desc DEBE SER UN ARCHIVO y no un directorio
   - Si recibe NULL, copia toda la particion (o el disco entero) en la CARPETA dest. Lo que hace es copiar archivo por archivo y los deja todos dentro de ese directorio
   Finalmente retorna 0 si hubo exito */
int cr_unload(unsigned disk, char* orig, char* dest);

