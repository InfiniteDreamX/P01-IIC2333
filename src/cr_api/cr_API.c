#include "cr_API.h"
#include <stdio.h>
#include <string.h>
#include "../utils/disk_utils.h"
#include "../utils/byte_utils.h"
#include "../utils/common_utils.h"


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
    if (!(strcmp(&mode, "r") == 0) && cr_exists(disk, filename)){
        uint8_t index_block_position_buffer[3];
        unsigned int index_block_position;

        
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
        
        unsigned long size = 0;
        size |= size_buff[0];
        size = size << 8;
        size |= size_buff[1];
        size = size << 8;
        size |= size_buff[2];
        size = size << 8;
        size |= size_buff[3];
        size = size << 8;
        size |= size_buff[4];
        size = size << 8;
        size |= size_buff[5];
        size = size << 8;
        size |= size_buff[6];
        size = size << 8;
        size |= size_buff[7];
        
        // A continuacion obtener direcciones para bloques de datos
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
        crFILE *crfile = malloc(sizeof(crFILE) + sizeof(unsigned int) * block_number);
        crfile->partition = disk;
        crfile->block = 0;
        crfile->byte = 0;
        crfile->mode = mode;
        crfile->references = references;
        crfile->size = size;
        memcpy(crfile->data_blocks, data_blocks, sizeof(unsigned int) * block_number);
        return crfile;
    }
    else if (!(strcmp(&mode, "w") == 0) && !cr_exists(disk, filename)){
        
    }
    else{
        exit_with_error("No se pudo abrir el archivo %s en modo %s.\n", filename, mode);
    }
} 


int cr_exists(unsigned disk, char* filename)
{
  int current_byte = 0;
  int filename_len = strlen(filename);
  if (filename_len > 29)
  {
      exit_with_error("Nombre de archivo muy largo: %i caracteres. (Maximo 29)", filename_len);
  }
  uint8_t int_filename[filename_len];
  while (current_byte < BLOCK_SIZE)
  {
      uint8_t info_buffer[1];
      read_block_partition_index(disk, 0, info_buffer, current_byte, 1);
      if (get_bit_from_byte(info_buffer[0], 7))
      {
          uint8_t name_buffer[29];
          read_block_partition_index(disk, 0, name_buffer, current_byte+3, 29);
          if (memcmp(filename, name_buffer, filename_len) == 0)
          {
              if (filename_len < 29) {
                if (name_buffer[filename_len] == 0) {
                    return 1;
                }
              } else {
                  return 1;
              }
          }
      }
      current_byte += 32;
  }
  return 0;
}