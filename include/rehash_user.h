#ifndef REHASH_USER_H
#define REHASH_USER_H

HashMap rehashUser(HashMap* hashMap){
    assert(hashMap);

    HashMap new_hashMap = hashMapCtor(hashMap->hash_func, hashMap->capacity * 2);
    new_hashMap.load_factor = hashMap->load_factor / 2;

    for (uint32_t i = 0; i < hashMap->capacity; i++){
        uint32_t elem_inx = (hashMap->lists + i)->list_elems[0].next_inx;

        while (elem_inx != 0){
            data_t data = (hashMap->lists + i)->list_elems[elem_inx].data;
            rehashAddElement(&new_hashMap, data);

            elem_inx = (hashMap->lists + i)->list_elems[elem_inx].next_inx;
        }
    }

    hashMapDtor(hashMap);

    return new_hashMap;
}

#endif
