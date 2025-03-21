#include "hashmap.h"

#include "string.h"
#include "../logging/logging.h"
#include "../debug/cabor_debug.h"

cabor_hash_map* cabor_create_hash_map(size_t initial_size)
{
    CABOR_NEW(cabor_hash_map, map);
    map->table = cabor_create_vector(initial_size, CABOR_MAP_ENTRY, false);
    map->table->size = initial_size;
    memset(map->table->vector_mem.mem, 0, cabor_get_map_entry_size() * map->table->size);
    return map;
}

void cabor_free_key(const char* key)
{
    cabor_allocation alloc =
    {
        .mem = key,
#ifdef CABOR_ENABLE_ALLOCATOR_FAT_POINTERS
        .size = strlen(key) + 1 // + null terminator
#endif
    };
    CABOR_FREE(&alloc);
}

void cabor_destroy_hash_map(cabor_hash_map* map)
{
    for (size_t i = 0; i < map->table->size; i++)
    {
        cabor_map_entry* entry = cabor_vector_get_map_entry(map->table, i);

        if (entry->key)
            cabor_free_key(entry->key);

        cabor_map_entry* next = entry->next;
        while (next)
        {
            cabor_map_entry* temp = next;
            next = next->next;
            if (temp->key)
                cabor_free_key(temp->key);
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

uint32_t cabor_hash_string_with_size(const char* str, size_t size)
{
    // FNV-1a hashing
    uint32_t hash = 2166136261u;
    while (*str)
    {
        if (str > size)
            break;
        hash ^= (uint8_t)(*str);
        hash *= 16777619u;
        str++;
    }
    return hash;
}

cabor_map_entry* cabor_map_insert(cabor_hash_map* map, const char* key, int value)
{
    const size_t table_size = map->table->size;
    uint32_t index;
    if (table_size > 0)
    {
        index = cabor_hash_string(key) % table_size;
    }
    else
    {
        index = cabor_hash_string(key) % table_size;
    }

    cabor_map_entry* entry = cabor_vector_get_map_entry(map->table, index);

    if (entry->key == NULL) // No collision, store (key,value) directly in the entry
    {
        entry->key = cabor_strdup(key);
        entry->value = value;
        entry->next = NULL;
        return entry;
    }

    while (true) // Check for collisions
    {
        if (strcmp(entry->key, key) == 0) // No collision, allow shadowing
        {
            entry->value = value;
            return entry;
        }

        if (entry->next)
            entry = entry->next;
        else
            break;
    }

    // collision, append the bucket with new value
    CABOR_NEW(cabor_map_entry, new_entry);
    new_entry->key = cabor_strdup(key);
    new_entry->value = value;
    new_entry->next = NULL;
    entry->next = new_entry;

    return new_entry;
}

int cabor_map_get(cabor_hash_map* map, const char* key, bool* found)
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
    return -1;
}

cabor_map_entry* cabor_map_get_entry(cabor_hash_map* map, const char* key, bool* found)
{
    const size_t table_size = map->table->size;
    uint32_t index = cabor_hash_string(key) % table_size;
    cabor_map_entry* entry = cabor_vector_get_map_entry(map->table, index);

    while (entry->key)
    {
        if (strcmp(entry->key, key) == 0)
        {
            *found = true;
            return entry;
        }
        entry = entry->next;
    }
    *found = false;
    return NULL;

}

size_t cabor_get_map_entry_size()
{
    return sizeof(cabor_map_entry);
}
