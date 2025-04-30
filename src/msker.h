#include <stdio.h>
#include <stdint.h>

#ifndef MSKER_H
#define MSKER_H

long get_file_size(FILE *fp);
uint32_t prng_seed(const char *hash);
void prng_next(uint32_t *prng_state);
void mskgen(const char *hash, FILE *file);

#endif
