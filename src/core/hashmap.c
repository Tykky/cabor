#include "hashmap.h"
#include "hashmap.h"

#include "string.h"

cabor_hash_map* cabor_create_hash_map()
{
    CABOR_NEW(cabor_hash_map, map);
    map->table = cabor_create_vector(CABOR_HASH_MAP_INITIAL_SIZE, CABOR_MAP_ENTRY, false);
    map->table->size = CABOR_HASH_MAP_INITIAL_SIZE;
    memset(map->table->vector_mem.mem, 0, get_cabor_map_entry_size() * map->table->size);
    return map;
}

void cabor_destroy_hash_map(cabor_hash_map* map)
{
    for (size_t i = 0; i < map->table->size; i++)
    {
        cabor_map_entry* entry = cabor_vector_get_map_entry(map->table, i);
        cabor_map_entry* next = entry->next;
        while (next)
        {
            cabor_map_entry* temp = entry;
            entry = entry->next;
            if (temp->key)
                CABOR_FREE(temp->key);
            CABOR_DELETE(cabor_map_entry, temp);
        }
    }

    cabor_destroy_vector(map->table);
    CABOR_DELETE(cabor_hash_map, map);
}

uint32_t cabor_hash_string(const char* str)
{
    // FNV-1a hashing
    uint32_t hash = 2166136261u;
    while (*str)
    {
        hash ^= (uint8_t)(*str);
        hash *= 16777619u;
        str++;
    }
    return hash;
}

void cabor_map_insert(cabor_hash_map* map, const char* key, uint32_t value)
{
    const size_t table_size = map->table->size;
    uint32_t index = cabor_hash_string(key) % table_size;

    cabor_map_entry* entry = cabor_vector_get_map_entry(map->table, index);

    if (entry->key == NULL) // No collision, store (key,value) directly in the entry
    {
        entry->key = cabor_strdup(key);
        entry->value = value;
        entry->next = NULL;
        return;
    }

    while (entry->key) // Check for collisions
    {
        if (strcmp(entry->key, key) == 0) // No collision, just update the value
        {
            entry->value = value;
            return;
        }
        entry = entry->next;
    }

    // collision, append the bucket with new value
    CABOR_NEW(cabor_map_entry, new_entry);
    new_entry->key = cabor_strdup(key);
    new_entry->value = value;
    new_entry->next = NULL;
    entry->next = new_entry;
}

uint32_t cabor_map_get(cabor_hash_map* map, const char* key, bool* found)
{
    const size_t table_size = map->table->size;
    uint32_t index = cabor_hash_string(key) % table_size;
    cabor_map_entry* entry = cabor_vector_get_map_entry(map->table, index);

    while (entry->key)
    {
        if (strcmp(entry->key, key) == 0)
        {
            *found = true;
            return entry->value;
        }
        entry = entry->next;
    }
    *found = false;
    return 0;
}

size_t get_cabor_map_entry_size()
{
    return sizeof(cabor_map_entry);
}
