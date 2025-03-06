#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "vector.h"

#define CABOR_HASH_MAP_INITIAL_SIZE 128

typedef struct cabor_map_entry_t
{
    char* key;
    uint32_t value;
    struct cabor_map_entry_t* next;
} cabor_map_entry;

typedef struct
{
    cabor_vector* table;
} cabor_hash_map;

cabor_hash_map* cabor_create_hash_map();
void cabor_destroy_hash_map(cabor_hash_map* map);

uint32_t cabor_hash_string(const char* str);
void cabor_map_insert(cabor_hash_map* map, const char* key, uint32_t value);
uint32_t cabor_map_get(cabor_hash_map* map, const char* key, bool* found);

size_t get_cabor_map_entry_size();
