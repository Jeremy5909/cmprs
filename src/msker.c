#include "msker.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

long get_file_size(FILE *fp) {
  fseek(fp, 0, SEEK_END);
  long sz = ftell(fp);
  fseek(fp, 0L, SEEK_SET);
  return sz;
}

uint32_t prng_seed(const char *hash) {
  char buf[9] = {0};
  strncpy(buf, hash, 8);
  return (uint32_t)strtoul(buf, NULL, 16);
}

void prng_next(uint32_t *prng_state) {
  *prng_state ^= (*prng_state << 13);
  *prng_state ^= (*prng_state >> 17);
  *prng_state ^= (*prng_state << 5);
}

void mskgen(const char *hash, FILE *file) {
  long file_size = get_file_size(file);
  uint32_t prng_state = prng_seed(hash);

  const size_t buffer_size = 16384; // 16 KB
  uint8_t *buffer = malloc(buffer_size);
  if (!buffer) {
    printf("Failed to allocate buffer\n");
    return;
  }

  long processed = 0;
  while (processed < file_size) {
    size_t to_read = (file_size - processed) > buffer_size
                         ? buffer_size
                         : (file_size - processed);
    size_t bytes_read = fread(buffer, 1, to_read, file);
    if (bytes_read != to_read) {
      printf("Failed to read chunk at position %ld\n", processed);
      break;
    }

    for (size_t i = 0; i < bytes_read; i++) {
      prng_next(&prng_state);
      uint8_t mask = prng_state & 0xFF;
      for (int j = 0; j < 8; j++) {
        if (((mask >> (7 - j)) & 1) == 0) {
          buffer[i] ^= (1 << (7 - j));
        }
      }
    }

    if (fseek(file, -bytes_read, SEEK_CUR) != 0) {
      printf("Failed to seek back for chunk at position %ld\n", processed);
      break;
    }

    size_t bytes_written = fwrite(buffer, 1, bytes_read, file);
    if (bytes_written != bytes_read) {
      printf("Failed to write chunk at position %ld\n", processed);
      break;
    }

    fflush(file);
    processed += bytes_read;
  }

  free(buffer);
}
