#include "cr_API.h"
#include <stdio.h>
#include <string.h>
#include "../utils/disk_utils.h"
#include "../utils/byte_utils.h"
#include "../utils/common_utils.h"

void cr_mount(char *diskname)
{
    disk_name = diskname;
}

void cr_ls(unsigned disk)
{
    if (disk < 1 || disk > 4)
    {
        exit_with_error("El numero de particion %d, no es valido.\n", disk);
    }
    printf("cr_ls(%d)--------------------\n", disk);
    uint8_t buff[32];
    int entry_size = 32;
    for (int i = 0; i < 256; i++)
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

int cr_write(crFILE* file_desc, void* buffer, int nbytes)
{
    if (file_desc->mode != 'w')
    {
        printf("El modo del archivo no es 'w'\n");
        return 0;
    }
    int current_byte = file_desc->byte + file_desc->block_number * BLOCK_SIZE;
    int to_write;
    int end_file = 0;
    if (nbytes > file_desc->size - current_byte)
    {
        to_write = file_desc->size - current_byte;
        end_file = 1;
    }
    else
    {
        to_write = nbytes;
    }

    // Escribo solo los bytes que faltan. Si escribo mas bytes de 
    // los que faltan -> termine el archivo
    int bytes_written = 0;
    while (bytes_written != to_write)
    {
        int written_qty;
        int next_block;
        int next_byte;

        // Lo que queda de bloque < lo que me queda por escribir
        if (BLOCK_SIZE - file_desc->byte <= to_write - bytes_written)
        {   
            written_qty = BLOCK_SIZE - file_desc->byte;
            next_block = 1;
            next_byte = 0;
        }
        else
        {
            written_qty = to_write - bytes_written;
            next_block = 0;
            next_byte = file_desc->byte + to_write - bytes_written;
        }
        uint8_t block_buffer[written_qty];
        memcpy(block_buffer, buffer + bytes_written, written_qty);
        write_block_partition_index(file_desc -> partition,
                                    file_desc->data_blocks[file_desc->block_number],
                                    block_buffer,
                                    file_desc -> byte,
                                    written_qty);

        bytes_written += written_qty;
        file_desc->block_number += next_block;
        file_desc->byte = next_byte;
                                    
        if (next_block) 
        {
            unsigned empty_block;
            empty_block = get_empty_block_direction(file_desc -> partition);
            // Escribo en el bloque indice
            // Tengo que cargar el empty block al buffer?
            write_block_index(file_desc -> index_block,
                             (uint8_t*)&empty_block,
                              12 + 4*file_desc->block_number,
                              4);
            file_desc->block = empty_block;
            file_desc->block_number = file_desc->block_number + 1;
            file_desc->data_blocks[file_desc->block_number] = empty_block;
        }
    }
    if (end_file)
    {
        file_desc->block = 0;
        file_desc->byte = 0;
    }
    return bytes_written;
}

crFILE *cr_open(unsigned disk, char *filename, char mode)
{
    printf("EN FUNCION\n");
    printf("disk: %d\n", disk);
    printf("filename: %s\n", filename);
    printf("mode: %c\n", mode);
    printf("mode check: %d\n", mode == 'r');
    printf("exists check: %d\n", cr_exists(disk, filename));
    if (mode == 'r' && cr_exists(disk, filename))
    {
        printf("EN CAMINO\n");
        char link1[] = "1/";
        char link2[] = "2/";
        char link3[] = "3/";
        char link4[] = "4/";
        if (!(memcmp(filename, link1, 2) && memcmp(filename, link2, 2) && memcmp(filename, link3, 2) && memcmp(filename, link4, 2)))
        {
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

        //  obtener posicion del index block
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
                read_block_partition_index(disk, 0, name_buffer, current_byte + 3, 29);
                if (memcmp(filename, name_buffer, filename_len) == 0)
                {
                    if (filename_len < 29) {
                        if (name_buffer[filename_len] == 0) {
                            was_located = 1;
                        }
                    } else {
                        was_located = 1;
                    }
                }
            }
            if (was_located)
            {
                printf("LOCATED\n");
                found_entry_byte = current_byte;
                current_byte = BLOCK_SIZE; // para terminar el while, y mantener el valor de la variable (en found_entry_byte)
            }
            else
            {
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
        int block_number = size / BLOCK_SIZE + (size % BLOCK_SIZE != 0);
        printf("block_number: %u\n", block_number);
        unsigned int data_blocks[size / BLOCK_SIZE + (size % BLOCK_SIZE != 0)];
        uint8_t indirect_block_buffer[4];
        read_block_index(index_block_position, indirect_block_buffer, BLOCK_SIZE - 4, 4);
        unsigned int indirect_block = (indirect_block_buffer[0] << 24) | (indirect_block_buffer[1] << 16) | (indirect_block_buffer[2] << 8) | (indirect_block_buffer[3]);
        printf("indirect_block: %u\n", indirect_block);
        for (int i = 0; i < block_number; i++)
        {
            uint8_t address_buffer[4];
            if (i < 2044)
            {
                read_block_index(index_block_position, address_buffer, 12 + 4 * i, 4);
            }
            else
            {
                read_block_index(indirect_block, address_buffer, (i - 2044) * 4, 4);
            }
            data_blocks[i] = (address_buffer[0] << 24) | (address_buffer[1] << 16) | (address_buffer[2] << 8) | (address_buffer[3]);
        }
        crFILE *crfile = malloc(sizeof(crFILE) + sizeof(unsigned int) * block_number);
        crfile->partition = disk;
        crfile->block = 0;
        crfile->index_block=index_block_position;
        crfile->block_number = block_number;
        crfile->byte = 0;
        crfile->mode = mode;
        crfile->references = references;
        crfile->size = size;
        memcpy(crfile->data_blocks, data_blocks, sizeof(unsigned int) * block_number);
        printf("mode: %c\n", crfile->mode);
        printf("compare mode: %d\n", crfile->mode == 'r');
        printf("partition: %u\n", crfile->partition);
        printf("references: %u\n", crfile->references);
        printf("size: %lu\n", crfile->size);
        printf("data_blocks[0]: %u\n", crfile->data_blocks[0]);
        printf("index_block: %u\n", crfile->index_block);
        return crfile;
    } else if (mode == 'w' && !cr_exists(disk, filename))
    {
        uint8_t directory_entry[32];
        int entry_size = 32;
        for(int i = 0; i < 256; i++)
        {
            read_block_partition_index(disk, 0, directory_entry, entry_size * i, 32);
            uint8_t first_byte = directory_entry[0];
            uint8_t valid_bit = first_byte >> 7;
            if (!valid_bit)
            {
                unsigned int block_direction = get_empty_block_direction(disk);
                uint8_t name_buff[29];
                memcpy(name_buff, filename, strlen(filename));
                name_buff[strlen(filename)] = '\0';
                uint8_t valid_index_buffer[3];
                memcpy((uint8_t*)valid_index_buffer,(uint8_t*)&block_direction,sizeof(uint8_t)*3);
                ReverseArray(valid_index_buffer, 3);
                valid_index_buffer[0] = set_bit_to_byte(valid_index_buffer[0], 7, 1);
                write_block_partition_index(disk, 0, valid_index_buffer, entry_size * i, 3); // 3 bytes en entrada directorio
                write_block_partition_index(disk, 0, name_buff, entry_size * i + 3, 29); // Nombre en entrada directorio
                unsigned int data_block_direction = get_empty_block_direction(disk);           
                uint8_t valid_data_index_buffer[4]; // 
                memcpy((uint8_t*)valid_data_index_buffer,(uint8_t*)&data_block_direction,sizeof(uint8_t)*4);
                ReverseArray(valid_data_index_buffer, 4);
                write_block_index(block_direction, valid_data_index_buffer, 12, 4); // direccion bloque de datos, en bloque indice
                unsigned int references = 1;
                uint8_t references_buffer[4];
                unsigned long size = 0;
                uint8_t size_buffer[8];
                memcpy((uint8_t*)references_buffer,(uint8_t*)&references,sizeof(uint8_t)*4);
                memcpy((uint8_t*)size_buffer,(uint8_t*)&size,sizeof(uint8_t)*8);
                ReverseArray(references_buffer, 4);
                ReverseArray(size_buffer, 8);
                write_block_index(block_direction, references_buffer, 0, 4); // referencias, en bloque indice
                write_block_index(block_direction, size_buffer, 4, 8); // tamaño, en bloque indice
                crFILE *crfile = malloc(sizeof(crFILE) + sizeof(unsigned int) * 1);
                crfile->mode = mode;
                crfile->partition = disk;
                crfile->references = references;
                crfile->size = size;
                crfile->byte = 0;
                crfile->index_block = block_direction;
                crfile->block = data_block_direction; // ES DECIR,  EN MODO 'w', block  REPRESENTA EL PUNTERO AL BLOQUE DE DATOS EN QUE SE ESTA ESCRIBIENDO
                printf("%c\n", crfile->mode);
                printf("%u\n", crfile->partition);
                printf("%u\n", crfile->references);
                printf("%lu\n", crfile->size);
                printf("%u\n", crfile->byte);
                printf("%u\n", crfile->index_block);
                printf("%u\n", crfile->block);
                return crfile;
            }
        }
    }
    else{
        exit_with_error("No se pudo abrir el archivo %s.\n", filename);
    }
}

int cr_exists(unsigned disk, char *filename)
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
            read_block_partition_index(disk, 0, name_buffer, current_byte + 3, 29);
            if (memcmp(filename, name_buffer, filename_len) == 0)
            {
                if (filename_len < 29)
                {
                    if (name_buffer[filename_len] == 0)
                    {
                        return 1;
                    }
                }
                else
                {
                    return 1;
                }
            }
        }
        current_byte += 32;
    }
    return 0;
}

int cr_read(crFILE *file_desc, void *buffer, int nbytes)
{
    if (file_desc->mode != 'r')
    {
        printf("El modo del archivo no es 'r'\n");
        return 0;
    }
    int current_byte = file_desc->byte + file_desc->block * BLOCK_SIZE;
    int to_read;
    int end_file = 0;
    if (nbytes > file_desc->size - current_byte)
    {
        to_read = file_desc->size - current_byte;
        end_file = 1;
    }
    else
    {
        to_read = nbytes;
    }
    int bytes_read = 0;
    while (bytes_read != to_read)
    {
        int read_qty;
        int next_block;
        int next_byte;
        if (bytes_read + BLOCK_SIZE - file_desc->byte <= to_read)
        {
            read_qty = BLOCK_SIZE - file_desc->byte;
            next_block = 1;
            next_byte = 0;
        }
        else
        {
            read_qty = to_read - bytes_read;
            next_block = 0;
            next_byte = file_desc->byte + to_read - bytes_read;
        }
        uint8_t block_buffer[read_qty];
        read_block_index(file_desc->data_blocks[file_desc->block], block_buffer, file_desc->byte, read_qty);
        memcpy(buffer + bytes_read, block_buffer, read_qty);
        bytes_read += read_qty;
        file_desc->block += next_block;
        file_desc->byte = next_byte;
    }
    if (end_file)
    {
        file_desc->block = 0;
        file_desc->byte = 0;
    }
    return bytes_read;
}

// Funcion privada para guardar toda una particion
void unload_disk(unsigned disk, char *dest)
{
    uint8_t buff[32];
    int entry_size = 32;
    for (int i = 0; i < 256; i++)
    {
        char name[32];
        read_block_partition_index(disk, 0, buff, entry_size * i, 32); // read_block_index_partition
        uint8_t first_byte = buff[0];
        uint8_t valid_bit = first_byte >> 7;
        if (valid_bit)
        {
            // recorrer desde byte 3 obteniendo el nombre -- acaba con cero
            read_block_partition_index(disk, 0, buff, entry_size * i + 3, 29);
            sprintf(name, "%s", buff);
            char new_name[32];
            if (name[1] == '/')
            {   
                memmove(new_name, name + 2, sizeof(name) - 2);
            }
            char new_dest[100];
            strcpy(new_dest, dest);
            strcat(new_dest, "/");
            if (name[1] == '/')
            {
                strcat(new_dest, new_name);
            }
            else
            {
                strcat(new_dest, name);
            }
            cr_unload(disk, name, new_dest);
        }
    }
}

int cr_unload(unsigned disk, char *orig, char *dest)
{
    if (orig == NULL)
    {
        if (disk == 0)
        {
            for (int i = 1; i < 5; i++)
            {
                unload_disk(i, dest);
            }
        }
        else
        {
            unload_disk(disk, dest);
        }
    }
    else
    {
        if (disk > 4 || disk < 1)
        {
            exit_with_error("Numero de particion incorrecto: %i\n", disk);
        }
        else
        {
            if (!cr_exists(disk, orig))
            {
                exit_with_error("El archivo %s no existe en la particion %i\n", orig, disk);
            }
            else
            {
                crFILE *file = cr_open(disk, orig, 'r');
                uint8_t *buffer = calloc(file->size, sizeof(uint8_t));
                cr_read(file, buffer, file->size);
                FILE *dest_file;
                if ((dest_file = fopen(dest, "wb")) == NULL)
                {
                    exit_with_error("Error abriendo el archivo de destino: %s\n", dest);
                }
                fwrite(buffer, sizeof(uint8_t), file->size, dest_file);
                fclose(dest_file);
                free(buffer);
                return 0;
            }
        }
    }
}