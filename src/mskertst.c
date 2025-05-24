#include "msker.h"
#include <stdio.h>

int main(int argc, char **argv) {

  // Get args
  if (argc != 2) {
    printf("Usage: %s <filename>\n", argv[0]);
    return 1;
  }
  char *input_file_name = argv[1];

  // Read file
  FILE *input_file;
  if ((input_file = fopen(input_file_name, "r+b")) == NULL) {
    printf("File '%s' not found\n", input_file_name);
    return 1;
  }

  // Change file
  mskgen("76ba652cbd2ef1931d0546ac1c9d8f12d21c81fad272b754975a0b1561dda275", input_file);

  // Close it
  fclose(input_file);
  return 0;
}