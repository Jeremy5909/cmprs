#include "msker.h"
#include <openssl/sha.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void sha256_from_file(FILE *file, char *output_hex) {
  unsigned char hash[SHA256_DIGEST_LENGTH];
  unsigned char buffer[4096];
  size_t bytes;
  SHA256_CTX sha256;

  SHA256_Init(&sha256);
  rewind(file); // ensure start of file

  while ((bytes = fread(buffer, 1, sizeof(buffer), file)) != 0) {
    SHA256_Update(&sha256, buffer, bytes);
  }
  SHA256_Final(hash, &sha256);

  for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
    sprintf(output_hex + (i * 2), "%02x", hash[i]);
  }
  output_hex[64] = '\0'; // null-terminate
}

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

  // Read file
  FILE *input_file;
  if ((input_file = fopen(input_file_name, "r+b")) == NULL) {
    printf("File '%s' not found\n", input_file_name);
    return 1;
  }

  // Compute SHA256
  char sha256_hex[65];
  sha256_from_file(input_file, sha256_hex);

  // Write SHA256 to file

  char *shasum_filename = build_shasum_filename(input_file_name);

  FILE *sha_file = fopen(shasum_filename, "w");
  if (sha_file) {
    fprintf(sha_file, "%s\n", sha256_hex);
    fclose(sha_file);
  } else {
    printf("Error writing shasum file\n");
    fclose(input_file);
    return 1;
  }

  // Change file
  rewind(input_file);
  mskgen(sha256_hex, input_file);

  // Close it
  fclose(input_file);
  return 0;
}
