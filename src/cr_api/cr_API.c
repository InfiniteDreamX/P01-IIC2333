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
    printf("EN FUNCION\n");
    printf("disk: %d\n", disk);
    printf("filename: %s\n", filename);
    printf("mode: %c\n", mode);
    printf("mode check: %d\n", mode == 'r');
    printf("exists check: %d\n", cr_exists(disk, filename));
    if (mode == 'r' && cr_exists(disk, filename)){
        printf("EN CAMINO\n");
        char link1[] = "1/";
        char link2[] = "2/";
        char link3[] = "3/";
        char link4[] = "4/";
        if (!(memcmp(filename, link1, 2) && memcmp(filename, link2, 2) && memcmp(filename, link3, 2) && memcmp(filename, link4, 2))) {
            char linked_name[strlen(filename) - 2];
            memcpy(linked_name, filename + 2, strlen(filename) - 2);
            linked_name[strlen(filename) - 2] = '\0';
            printf("memcmp: %d\n", memcmp(linked_name, "Baroque.mp3", strlen(linked_name)));
            printf("pre-exists check (in 1): %d\n", cr_exists(1, "Baroque.mp3"));
            printf("linked_name: %s\n", linked_name);
            unsigned int linked_disk = filename[0] - '0';
            printf("%d\n", linked_disk);
            crFILE *crfile = cr_open(linked_disk, linked_name, 'r');
            return crfile;
        }

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
                printf("LOCATED\n");
                found_entry_byte = current_byte;
                current_byte = BLOCK_SIZE; // para terminar el while, y mantener el valor de la variable (en found_entry_byte)
            } else {
                current_byte += 32;
            }
        }
        read_block_partition_index(disk, 0, index_block_position_buffer, found_entry_byte, 3);
        index_block_position_buffer[0] = set_bit_to_byte(index_block_position_buffer[0], 7, 0);
        index_block_position = (index_block_position_buffer[0] << 16) | (index_block_position_buffer[1] << 8) | (index_block_position_buffer[2]);
        printf("index_block_partition: %u\n", index_block_position);
        uint8_t references_buff[4];
        uint8_t size_buff[8];
        read_block_index(index_block_position, references_buff, 0, 4);
        read_block_index(index_block_position, size_buff, 4, 8);
        unsigned int references = (references_buff[0] << 24) | (references_buff[1] << 16) | (references_buff[2] << 8) | (references_buff[3]); 
        printf("references: %u\n", references);
        // unsigned long size = (size_buff[0] << 56) | (size_buff[1] << 48) | (size_buff[2] << 40) | (size_buff[3] << 32) | (size_buff[4] << 24) | (size_buff[5] << 16) | (size_buff[6] << 8) | (size_buff[7]);
        
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
        
        printf("size: %lu\n", size);
        // A continuacion obtener direcciones para bloques de datos
        int block_number = size/BLOCK_SIZE + (size % BLOCK_SIZE != 0);
        printf("block_number: %u\n", block_number);
        unsigned int data_blocks[size/BLOCK_SIZE + (size % BLOCK_SIZE != 0)];
        uint8_t indirect_block_buffer[4];
        read_block_index(index_block_position, indirect_block_buffer, BLOCK_SIZE - 4, 4);
        unsigned int indirect_block = (indirect_block_buffer[0] << 24) | (indirect_block_buffer[1] << 16) | (indirect_block_buffer[2] << 8) | (indirect_block_buffer[3]);
        printf("indirect_block: %u\n", indirect_block);
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
        printf("%c\n", crfile->mode);
        printf("compare mode: %d\n", crfile->mode == 'r');
        printf("%u\n", crfile->partition);
        printf("%u\n", crfile->references);
        printf("%lu\n", crfile->size);
        printf("%u\n", crfile->data_blocks[0]);
        return crfile;
    }
    else if (mode == 'w' && !cr_exists(disk, filename)){
        uint8_t directory_entry[32];
        int entry_size = 32;
        for(int i = 0; i < 256; i++)
        {
            read_block_partition_index(disk, 0, directory_entry, entry_size * i, 32);
            uint8_t first_byte = directory_entry[0];
            uint8_t valid_bit = first_byte >> 7;
            if (!valid_bit)
            {
                unsigned int block_direction = (disk - 1) * BLOCK_SIZE; // 0
                for (int k = 0; k < BLOCK_SIZE; k++) { 
                    uint8_t bitmap_byte[1];
                    read_block_partition_index(disk, 1, bitmap_byte, k, 1);
                    for (int j = 0; j < 8; j++) {
                        unsigned int block_used = get_bit_from_byte(bitmap_byte[0], 7 - j);
                        if (!block_used) {
                            uint8_t name_buff[29];
                            memcpy(name_buff, filename, strlen(filename));
                            name_buff[strlen(filename)] = '\0';
                            uint8_t valid_index_buffer[3];
                            memcpy((uint8_t*)valid_index_buffer,(uint8_t*)&block_direction,sizeof(uint8_t)*3);
                            uint8_t temp = valid_index_buffer[0];
                            valid_index_buffer[0] = valid_index_buffer[2];
                            valid_index_buffer[2] = temp;
                            valid_index_buffer[0] = set_bit_to_byte(valid_index_buffer[0], 7, 1);
                            write_block_partition_index(disk, 0, valid_index_buffer, entry_size * i, 3); // 3 bytes en entrada directorio
                            write_block_partition_index(disk, 0, name_buff, entry_size * i + 3, 29); // Nombre en entrada directorio
                            // obtener y escribir datos en bloque indice
                            unsigned int data_block_direction = (disk - 1) * BLOCK_SIZE; // 0
                            for (int d = 0; d < BLOCK_SIZE; d++) {
                                uint8_t bitmap_byte[1]; // cambiar nombre?
                                read_block_partition_index(disk, 1, bitmap_byte, d, 1);
                                for (int n = 0; n < 8; n++) {
                                    unsigned int data_block_used = get_bit_from_byte(bitmap_byte[0], 7 - n);
                                    if (!data_block_used) {
                                        uint8_t valid_index_buffer[4];
                                        memcpy((uint8_t*)valid_index_buffer,(uint8_t*)&data_block_direction,sizeof(uint8_t)*4);
                                        for(int x = 0; x<2; x++) {
                                            uint8_t temp = valid_index_buffer[x];
                                            valid_index_buffer[x] = valid_index_buffer[4-x-1];
                                            valid_index_buffer[4-x-1] = temp;        
                                        }
                                        // PARTIR LEYENDO DESDE ACÁ, VIENDO COMENTARIOS EN LAS PROXIMAS LINEAS, PENSANDO LOS PUNTEROS Y DEFINIENDO BIEN LAS LINEAS QUE QUEDAN.
                                        // write_block_index(block_direction, valid_index_buffer, 12, 4); // cachar si estoy usando bien los numeros de bloques / direcciones. Tal vez, dependiendo del bloque, voy a tener que sumarle un (disk - 1) * BLOCK_SIZE
                                        write_block_index(block_direction, valid_index_buffer, 12, 4); // tal vez block direction tenía que tener (disk - 1) * BLOCK_SIZE, y acá no usar funcion con partition.
                                        unsigned int references = 1;
                                        uint8_t references_buffer[4];
                                        unsigned long size = 0;
                                        uint8_t size_buffer[8];
                                        memcpy((uint8_t*)references_buffer,(uint8_t*)&references,sizeof(uint8_t)*4);
                                        memcpy((uint8_t*)size_buffer,(uint8_t*)&size,sizeof(uint8_t)*8);
                                        // escribir bien (usar funcion para esto)
                                        // https://stackoverflow.com/questions/47745149/reverse-an-integer-array-in-c
                                        for(int x = 0; x<2; x++) {
                                            uint8_t temp = references_buffer[x];
                                            references_buffer[x] = references_buffer[4-x-1];
                                            references_buffer[4-x-1] = temp;        
                                        }
                                        for(int x = 0; x<4; x++) {
                                            uint8_t temp = size_buffer[x];
                                            size_buffer[x] = size_buffer[8-x-1];
                                            size_buffer[8-x-1] = temp;        
                                        }
                                        write_block_index(block_direction, references_buffer, 0, 4); // referencias
                                        write_block_index(block_direction, size_buffer, 4, 8); // tamaño
                                        crFILE *crfile = malloc(sizeof(crFILE) + sizeof(unsigned int) * 1);
                                        crfile->mode = mode;
                                        crfile->partition = disk;
                                        crfile->references = references;
                                        crfile->size = size;
                                        crfile->byte = 0;
                                        crfile->block = data_block_direction; // ES DECIR,  EN MODO 'w', block  REPRESENTA EL PUNTERO AL BLOQUE DE DATOS EN QUE SE ESTA ESCRIBIENDO
                                    }
                                    data_block_direction += 1;
                                }
                            }
                        }
                        block_direction += 1;
                    }
                }
            }
        }
    }
    else{
        printf("ERROR");
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