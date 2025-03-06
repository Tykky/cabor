#include "hashmap_test.h"

#include <stdio.h>
#include "../../core/hashmap.h"

int cabor_unit_test_hashmap_insert_and_get()
{
    cabor_hash_map* map = cabor_create_hash_map();

    cabor_map_insert(map, "key1", 100);
    cabor_map_insert(map, "key2", 200);
    cabor_map_insert(map, "key3", 300);

    bool found = false;
    int res = 0;
    CABOR_CHECK_EQUALS(cabor_map_get(map, "key1", &found), 100, res);
    CABOR_CHECK_EQUALS(found, true, res);
    CABOR_CHECK_EQUALS(cabor_map_get(map, "key2", &found), 200, res);
    CABOR_CHECK_EQUALS(found, true, res);
    CABOR_CHECK_EQUALS(cabor_map_get(map, "key3", &found), 300, res);
    CABOR_CHECK_EQUALS(found, true, res);

    cabor_destroy_hash_map(map);

    return res;
}

int cabor_unit_test_hashmap_collison_test()
{
    cabor_hash_map* map = cabor_create_hash_map();

    cabor_allocation key_alloc = CABOR_MALLOC(9);
    char* temp_key = key_alloc.mem;

    // Inserting this many elements in the hashmap will force it to have collisions
    for (int i = 0; i < CABOR_HASH_MAP_INITIAL_SIZE * 4; i++)
    {
        snprintf(temp_key, 9, "key%05d", i);
        cabor_map_insert(map, temp_key, i * 100);
    }

    int res = 0;

    // Now even though we have A LOT of collision we should be able to retrieve all of it
    for (int i = 0; i < CABOR_HASH_MAP_INITIAL_SIZE * 4; i++)
    {
        snprintf(temp_key, 9, "key%05d", i);
        bool found = false;
        int value = cabor_map_get(map, temp_key, &found);
        CABOR_CHECK_EQUALS(value, i * 100, res);
        CABOR_CHECK_EQUALS(found, true, res);
    }

    cabor_free_key(temp_key);
    cabor_destroy_hash_map(map);

    return res;
}

