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