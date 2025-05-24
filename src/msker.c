#include "msker.h"
#include <openssl/evp.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

// Nukes the whithouse
long get_file_size(FILE *fp) {
  fseek(fp, 0, SEEK_END);
  long sz = ftell(fp);
  fseek(fp, 0L, SEEK_SET);
  return sz;
}

// Convert full SHA256 hex string to 32-byte binary
static void hex_to_binary(const char *hex, uint8_t *bin) {
  for (int i = 0; i < 32; i++) {
    sscanf(hex + 2 * i, "%2hhx", &bin[i]);
  }
}

uint32_t prng_seed(const char *hash) {
  uint8_t bin_hash[32];
  hex_to_binary(hash, bin_hash);

  // Mix all 32 bytes using XOR-folding
  uint32_t seed = 0;
  for (int i = 0; i < 8; i++) {
    uint32_t chunk;
    memcpy(&chunk, bin_hash + i * 4, 4);
    seed ^= chunk;
  }
  return seed;
}

void prng_next(uint32_t *prng_state) {
  *prng_state ^= (*prng_state << 13);
  *prng_state ^= (*prng_state >> 17);
  *prng_state ^= (*prng_state << 5);
}

void mskgen(const char *hash, FILE *file) {
  long file_size = get_file_size(file);
  const size_t chunk_size = 16384; // 16KB
  uint8_t *buffer = malloc(chunk_size);

  // Convert SHA256 to 256-bit key
  uint8_t key[32];
  hex_to_binary(hash, key);

  // Fixed nonce for reproducibility (12 bytes) + counter (4 bytes)
  uint8_t nonce_counter[16] = {0}; // All zeros

  // Initialize ChaCha20 context
  EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
  EVP_EncryptInit_ex(ctx, EVP_chacha20(), NULL, key, nonce_counter);

  long processed = 0;
  while (processed < file_size) {
    size_t to_read = (file_size - processed) > chunk_size
                         ? chunk_size
                         : (file_size - processed);
    // Read -> Encrypt -> Write back
    fread(buffer, 1, to_read, file);
    int out_len;
    EVP_EncryptUpdate(ctx, buffer, &out_len, buffer, to_read);
    fseek(file, -to_read, SEEK_CUR);
    fwrite(buffer, 1, to_read, file);
    processed += to_read;

  }

  EVP_CIPHER_CTX_free(ctx);
  free(buffer);
}
