#include <stdio.h>
#include <stdlib.h>
#include "cr_API.h"

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Cantidad incorrecta de parametros: %i\n", argc);
        return 1;
    }
    cr_mount(argv[1]);
    printf("%s\n", disk_name);
    // Comenzar aqui
}