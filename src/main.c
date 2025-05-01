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

  mskgen("deadbeefcafebabe", input_file);

  fclose(input_file);
  return 0;
}
