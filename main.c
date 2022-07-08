//
//  main.c
//  HashMap实现
//
//  Created by zhaofei on 2022/7/7.
//

#include <stdio.h>
#include "HashMap.h"

int main(int argc, const char * argv[]) {
    // insert code here...
    HashMap *hashMap = createHashMap(NULL, NULL);
    hashMap->put(hashMap, "name", "Monkey Z");
    hashMap->put(hashMap, "age", "29");
    hashMap->put(hashMap, "sex", "男");
    hashMap->put(hashMap, "height", "172");
    
    printf("exists age: %d\n", hashMap->exists(hashMap, "age"));
    hashMap->remove(hashMap, "age");

    HashMapIterator *iterator = createHashMapIterator(hashMap);
    while (hashNextHashMapIterator(iterator)) {
        iterator = nextHashMapIterator(iterator);
        printf("{ key: %s, value: %s, hashCode: %d}\n",
               (char *)iterator->entry->key,
               (char *)iterator->entry->value,
               iterator->hashCode);
    }
    hashMap->clear(hashMap);
    freeHashMapIterator(iterator);
    return 0;
}
