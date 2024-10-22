#include "bloom_filter.h"
#include <stddef.h>

uint64_t calc_hash(const char* str, uint64_t modulus, uint64_t seed) {
  uint64_t hash = 0;
  uint64_t mult = 1;
  int pos = 0;
  while (str[pos] != '\0') {
    mult *= seed;
    hash += str[pos] * mult;
    ++pos;
  }
  hash %= modulus;
  return hash;
    // TODO: Write any hash function for further usage
}

void bloom_init(struct BloomFilter* bloom_filter, uint64_t set_size, hash_fn_t hash_fn, uint64_t hash_fn_count) {
  bloom_filter->set = (uint64_t*)malloc((set_size + 63) / 64);
  bloom_filter->set_size = set_size;
  bloom_filter->hash_fn = hash_fn;
  bloom_filter->hash_fn_count = hash_fn_count;
    // TODO: Initialize bloom filter
}

void bloom_destroy(struct BloomFilter* bloom_filter) {
  free(bloom_filter->set);
  bloom_filter->set = NULL;
  bloom_filter->set_size = 0;
  bloom_filter->hash_fn = NULL;
  bloom_filter->hash_fn_count = 0;
    // TODO: Free memory if needed
}

void bloom_insert(struct BloomFilter* bloom_filter, Key key) {
  for (uint64_t i = 0; i < bloom_filter->hash_fn_count; ++i) {
    uint64_t hash = bloom_filter->hash_fn(key, bloom_filter->set_size, 17 * i + 3);
    uint64_t* ptr = bloom_filter->set + (hash / 64);
    uint64_t mask = 1;
    uint64_t shift = hash % 64;
    mask <<= shift;
    *ptr ^= mask;
  }
    // TODO: Insert key into set
}

bool bloom_check(struct BloomFilter* bloom_filter, Key key) {
  for (uint64_t i = 0; i < bloom_filter->hash_fn_count; ++i) {
    uint64_t hash = bloom_filter->hash_fn(key, bloom_filter->set_size, 17 * i + 3);
    uint64_t* ptr = bloom_filter->set + (hash / 64);
    uint64_t mask = 1;
    uint64_t shift = hash % 64;
    mask <<= shift;
    if ((*ptr & mask) == 0) {
        return false;
    }
  }
  return true;
    // TODO: Check if key exists in set
}
