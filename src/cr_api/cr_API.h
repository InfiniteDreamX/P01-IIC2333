#pragma once
#include <stdlib.h>

typedef struct cr_file crFILE;
struct cr_file
{
   unsigned int *start; // Puntero al inicio del archivo
   unsigned int *current;  // Puntero al lugar actual de lectura
   char mode; // r o w
   unsigned long size; // size del archivo, son 8 bytes
   unsigned int references; // Cantidad total de referencias
};
char* disk_name;
void cr_mount(char* diskname);

void cr_ls(unsigned disk);

int cr_exists(unsigned disk, char* filename);

