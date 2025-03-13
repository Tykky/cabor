#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "vector.h"

typedef struct cabor_map_entry_t
{
    char* key;
    int value;
    struct cabor_map_entry_t* next;
} cabor_map_entry;

typedef struct
{
    cabor_vector* table;
} cabor_hash_map;

cabor_hash_map* cabor_create_hash_map(size_t initial_size);
void cabor_free_key(const char* key);
void cabor_destroy_hash_map(cabor_hash_map* map);

uint32_t cabor_hash_string(const char* str);
cabor_map_entry* cabor_map_insert(cabor_hash_map* map, const char* key, int value);
int cabor_map_get(cabor_hash_map* map, const char* key, bool* found);
cabor_map_entry* cabor_map_get_entry(cabor_hash_map* map, const char* key, bool* found);

size_t cabor_get_map_entry_size();
