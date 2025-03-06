#include "hashmap_test.h"

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

