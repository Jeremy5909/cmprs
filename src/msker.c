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
  // Takes first 8 hex characters of hash and convert to seed
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

  for (long i = 0; i < file_size; i++) {
    uint8_t byte;

    if (fread(&byte, 1, 1, file) != 1) {
      printf("Failed to read byte at position %ld\n", i);
      break;
    }

    prng_next(&prng_state);

    // Keep last 8 bits
    uint8_t mask = prng_state & 0xFF;

    // Flip bit if corresponding bit in mask is 0
    byte ^= ~mask;

    fseek(file, -1, SEEK_CUR);
    fwrite(&byte, 1, 1, file);
    fflush(file);
  }
}
