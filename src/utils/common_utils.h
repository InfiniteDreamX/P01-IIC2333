#pragma once

// Define funciones para operaciones comunes, validas para cualquier contexto.

// Bota el programa mostrando un mensaje de error.
// Es buena practica botar el programa cuando se encuentran valores incorrectos.
// Ver ejemplos de uso en base_print de byte_utils.c
void exit_with_error(char* error_format, ...);