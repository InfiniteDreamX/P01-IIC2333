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
    printf("cr_ls(%d)--------------------\n"); 
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
    if (mode == "r" && cr_exists(disk, filename)){
        uint8_t index_block_position_buffer[3];
        unsigned int index_block_position;

        // falta obtener posicion del index block
        int current_byte = 0;
        int filename_len = strlen(filename);
        uint8_t info_buffer[1];
        uint8_t name_buffer[29];
        int was_located = 0;
        int found_entry_byte = 0;
        while (current_byte < BLOCK_SIZE)
        {
            // uint8_t info_buffer[1];
            read_block_partition_index(disk, 0, info_buffer, current_byte, 1);
            if (get_bit_from_byte(info_buffer[0], 7))
            {
                // uint8_t name_buffer[29];
                read_block_partition_index(disk, 0, name_buffer, current_byte+3, 29);
                if (memcmp(filename, name_buffer, filename_len) == 0)
                {
                    if (filename_len < 29) {
                        if (name_buffer[filename_len] == 0) {
                            // return 1;
                            was_located = 1;
                        }
                    } else {
                        // return 1;
                        was_located = 1;
                    }
                }
            }
            if (was_located) {
                found_entry_byte = current_byte;
                current_byte = BLOCK_SIZE; // para terminar el while, y mantener el valor de la variable (en found_entry_byte)
            } else {
                current_byte += 32;
            }
        }
        read_block_partition_index(disk, 0, index_block_position_buffer, found_entry_byte, 3);
        index_block_position_buffer[0] = set_bit_to_byte(index_block_position_buffer[0], 7, 0);
        index_block_position = (index_block_position_buffer[0] << 16) | (index_block_position_buffer[1] << 8) | (index_block_position_buffer[2]);

        uint8_t references_buff[4];
        uint8_t size_buff[8];
        read_block_index(index_block_position, references_buff, 0, 4);
        read_block_index(index_block_position, size_buff, 4, 8);
        unsigned int references = (references_buff[0] << 24) | (references_buff[1] << 16) | (references_buff[2] << 8) | (references_buff[3]); 
        unsigned long size = (size_buff[0] << 56) | (size_buff[1] << 48) | (size_buff[2] << 40) | (size_buff[3] << 32) | (size_buff[4] << 24) | (size_buff[5] << 16) | (size_buff[6] << 8) | (size_buff[7]);
    
        // A continuacion obtener direcciones para bloques de datos
        // a checked se le va sumando la capacidad ya contenida en los bloques incluidos en el array
        unsigned long checked = 0;
        int block_number = size/BLOCK_SIZE + (size % BLOCK_SIZE != 0);
        unsigned int data_blocks[size/BLOCK_SIZE + (size % BLOCK_SIZE != 0)];
        uint8_t indirect_block_buffer[4];
        read_block_index(index_block_position, indirect_block_buffer, BLOCK_SIZE - 4, 4);
        unsigned int indirect_block = (indirect_block_buffer[0] << 24) | (indirect_block_buffer[1] << 16) | (indirect_block_buffer[2] << 8) | (indirect_block_buffer[3]);
        for (int i = 0; i < block_number; i++) {
            uint8_t address_buffer[4];
            if (i < 2044) {
                read_block_index(index_block_position, address_buffer, 12 + 4 * i, 4);
            } else {
                read_block_index(indirect_block, address_buffer, (i - 2044) * 4, 4);
            }
            data_blocks[i] = (address_buffer[0] << 24) | (address_buffer[1] << 16) | (address_buffer[2] << 8) | (address_buffer[3]);
        }

        
    }
    else if (mode == "w" && !cr_exists(disk, filename)){
        
    }
    else{
        exit_with_error("No se pudo abrir el archivo %s en modo %s.\n", filename, mode);
    } 
}