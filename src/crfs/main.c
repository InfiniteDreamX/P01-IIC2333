#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "../cr_api/cr_API.h"
#include "../utils/byte_utils.h"

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
            printf("Introduce el numero de particion (1-4):");
            scanf("%u", &disk);
            char filename[29];
            printf("Introduce el nombre del archivo:");
            scanf("%s", filename);
            printf("%i\n", cr_exists(disk, filename));
        }
        if (strcmp(command, "cr_open") == 0)
        {
            crFILE* guides = cr_open(1, "guides.txt", 'r');
            uint8_t bytes[100];
            uint8_t bytes2[100];
            int bytes_read = cr_read(guides, bytes, 100);
            printf("Se leyeron %i bytes\n", bytes_read);
            print_bytes_ascii(bytes, 100);
            int bytes2_read = cr_read(guides, bytes2, 100);
            printf("Se leyeron %i bytes\n", bytes2_read);
            print_bytes_ascii(bytes2, 100);



        }
        if (strcmp(command, "cr_ls") == 0)
        {
            cr_ls(1);
        }
        if (strcmp(command, "exit") == 0)
        {
            printf("Adios!\n");
            exit(0);
        }
    }

}