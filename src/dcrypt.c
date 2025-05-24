#include "msker.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *build_shasum_filename(const char *prefix) {
  char shasum_filename_seperator = '-';
  char *suffix = "sha256sum";

  size_t len = strlen(prefix) + 1 + strlen(suffix) + 1;
  char *result = malloc(len);
  if (!result)
    return NULL;

  strcpy(result, prefix);

  char temp[2] = {shasum_filename_seperator, '\0'};
  strcat(result, temp);

  strcat(result, suffix);

  return result;
}

int main(int argc, char **argv) {

  // Get args
  if (argc != 2) {
    printf("Usage: %s <filename>\n", argv[0]);
    return 1;
  }
  char *input_file_name = argv[1];

  // Read SHA256 from file
  char *shasum_filename = build_shasum_filename(input_file_name);

  FILE *sha_file = fopen(shasum_filename, "r");
  if (!sha_file) {
    printf("Error: could not open 'shasum' for reading\n");
    return 1;
  }

  char sha256_hex[65];
  if (fgets(sha256_hex, sizeof(sha256_hex), sha_file) == NULL) {
    printf("Error: 'shasum' file is empty or invalid\n");
    fclose(sha_file);
    return 1;
  }
  // Remove newline if present
  sha256_hex[strcspn(sha256_hex, "\r\n")] = '\0';
  fclose(sha_file);

  // Read target file
  FILE *input_file;
  if ((input_file = fopen(input_file_name, "r+b")) == NULL) {
    printf("File '%s' not found\n", input_file_name);
    return 1;
  }

  // Use SHA256 string from file
  mskgen(sha256_hex, input_file);

  // Close file
  fclose(input_file);
  return 0;
}
