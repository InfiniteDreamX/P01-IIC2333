#include "cr_API.h"
#include "../utils/disk_utils.h"
#include "../utils/common_utils.h"
#include <stdio.h>

void cr_mount(char* diskname)
{
    disk_name = diskname;
}

void cr_ls(unsigned disk)
{
    if(disk < 1 || disk > 4){
        exit_with_error("El numero de particion %d, no es valido.\n", disk);
    }
    printf("cr_ls(%d)--------------------\n", disk); 
    uint8_t buff[32];
    int entry_size = 32;
    for(int i = 0; i < 256; i++)
    {
        read_block_partition_index(disk, 0, buff, entry_size * i, 32); // read_block_index_partition
        uint8_t first_byte = buff[0];
        uint8_t valid_bit = first_byte >> 7;
        if (valid_bit)
        {
            // recorrer desde byte 3 obteniendo el nombre -- acaba con cero
            int character = 3;
            uint8_t character_byte = buff[character];
            while (character_byte)
            {
                printf("%c", character_byte);
                character += 1;
                character_byte = buff[character];
            }
            printf("\n");
        }
    }
    printf("----------------------------\n"); 
}

crFILE* cr_open(unsigned disk, char* filename, char mode)
{

}