#pragma once
#include <stdlib.h>

typedef struct cr_file crFILE;
struct cr_file
{
   /*
   unsigned int *start; // Puntero al inicio del archivo
   unsigned int *current;  // Puntero al lugar actual de lectura
   char mode; // r o w
   unsigned long size; // size del archivo, son 8 bytes
   unsigned int references; // Cantidad total de referencias
   */
   
   unsigned int partition; // particion donde se encuentra el archivo
   unsigned int block; // bloque de datos actual
   unsigned int byte;  // indice dentro del bloque de datos actual
   char mode; // r o w
   unsigned int references; // 4 bytes para cantidad de hardlinks
   unsigned long size; // 8 bytes para tamaño de archivo
   unsigned int data_blocks[]; // Array con las direcciones de los bloques de datos del archivo
};
char* disk_name;
void cr_mount(char* diskname);

void cr_ls(unsigned disk);

int cr_exists(unsigned disk, char* filename);

