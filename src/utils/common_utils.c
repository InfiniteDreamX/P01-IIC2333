#include "common_utils.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void exit_with_error(char* error_format, ...){

  int format_size = strlen(error_format);
  char buff[format_size + 7];

  strcpy(buff, "ERROR: ");
  strcat(buff, error_format);

  va_list args;
  va_start(args, error_format);

  vfprintf(stderr, buff, args);
  fprintf(stderr, "\n");

  va_end(args);
  exit(1);
}

// https://stackoverflow.com/questions/47745149/reverse-an-integer-array-in-c
void ReverseArray(int arr[], int size)
{
    for (int i = 0; i < size/2; ++i)
    {
        int temp = arr[i];
        arr[i] = arr[size - 1 - i];
        arr[size - 1 - i] = temp;
    }
}