#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../cr_api/cr_API.h"

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Cantidad incorrecta de parametros: %i\n", argc);
        return 1;
    }
    cr_mount(argv[1]);

    printf("Utilizando el disco %s\n", disk_name);
    while (1)
    {
        char command[100];
        printf("Hola! Introduce el nombre del comando que quieres utilizar (sin los parametros)\n");
        scanf("%s", command);
        if (strcmp(command, "cr_exists") == 0)
        {
            unsigned disk;
            printf("Introduce el numero de particion (1-4): ");
            scanf("%u", &disk);
            char filename[29];
            printf("Introduce el nombre del archivo: ");
            scanf("%s", filename);
            printf("%i\n", cr_exists(disk, filename));
        }
        if (strcmp(command, "cr_bitmap") == 0)
        {
            unsigned disk;
            printf("Introduce el numero de particion (0-4): ");
            scanf("%u", &disk);
            printf("DEBUG: Se introdujo la siguiente particion: %d\n", disk);

            char ans[1];
            printf("Formato hexadecimal? [y/n]: ");
            scanf("%s", ans);
            if (strcmp(ans, "y") == 0) {
                cr_bitmap(disk, true);
            } else 
            {
                cr_bitmap(disk, false);
            }
        }
        if (strcmp(command, "exit") == 0)
        {
            printf("Adios!\n");
            exit(0);
        }
    }

}