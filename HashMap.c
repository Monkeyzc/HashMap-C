//
//  HashMap.c
//  HashMap实现
//
//  Created by zhaofei on 2022/7/8.
//

#include "HashMap.h"
#include <stdlib.h>

// 默认哈希函数
static int defaultHashCode(HashMap *hashMap, void *key);
// 默认判等函数
static int defaultEqual(void *key1, void *key2);
// 默认添加键值对的函数
static void defaultPut(HashMap *hashMap, void *key, void *value);
// 默认获取键对应值的函数
static void* defaultGet(HashMap *hashMap, void *key);
// 默认删除键的函数
static int defaultRemove(HashMap *hashMap, void *key);
// 默认清空Map的函数
static void defaultClear(HashMap *hashMap);
// 默认判断键值是否存在的函数
static int defaultExists(HashMap *hashMap, void *key);

// 重新构建
static void resetHashMap(HashMap *hashMap, int listSize);

// 创建哈希结构迭代器
HashMapIterator *createHashMapIterator(HashMap *hashMap) {
    HashMapIterator *iterator = (HashMapIterator *)malloc(sizeof(struct hashMapIterator));
    iterator->hashMap = hashMap;
    iterator->count = 0;
    iterator->hashCode = -1;
    iterator->entry = NULL;
    return iterator;
}

// 哈希结构迭代器 是否有下一个
int hashNextHashMapIterator(HashMapIterator *iterator) {
    return iterator->count < iterator->hashMap->size ? 1 : 0;
}

// 迭代到下一次
HashMapIterator *nextHashMapIterator(HashMapIterator *iterator) {
    if (hashNextHashMapIterator(iterator)) {
        if (iterator->entry != NULL && iterator->entry->next != NULL) {
            iterator->count++;
            iterator->entry = iterator->entry->next;
            return iterator;
        }
    }
    
    while (++iterator->hashCode < iterator->hashMap->listSize) {
        Entry *entry = &iterator->hashMap->list[iterator->hashCode]; // 为什么?
        if (entry->key != NULL) {
            iterator->count++;
            iterator->entry = entry;
            break;
        }
    }
    return iterator;
}

void freeHashMapIterator(HashMapIterator *iterator) {
    free(iterator);
    iterator = NULL;
}

// 默认哈希函数
int defaultHashCode(HashMap *hashMap, void *key) {
//    char *k = (char *)key;
//    unsigned long h = 0;
//    while (*k) {
//        h = (h << 4) + *k++;
//        unsigned long g = h & 0xF0000000L;
//        if (g) {
//            h ^= g >> 24;
//        }
//        h &= ~g;
//    }
//    return h % hashMap->listSize;
    char *str = (char *)key;
    int hash = 0;
    while (*str) {
        hash = (*str++) + (hash << 6) + (hash << 16) - hash;
    }

    return (hash & 0x7FFFFFF) % hashMap->listSize;
}

int defaultEqual(void *key1, void *key2) {
    return key1 == key2;
}

HashMap *createHashMap(HashCode hashCode, Equal equal) {
    HashMap *hashMap = (HashMap *)malloc(sizeof(struct hashMap));
    hashMap->size = 0;
    hashMap->listSize = 8;
    hashMap->hashCode = hashCode == NULL ? defaultHashCode : hashCode;
    hashMap->equal = equal == NULL ? defaultEqual : equal;
    hashMap->put = defaultPut;
    hashMap->get = defaultGet;
    hashMap->remove = defaultRemove;
    hashMap->clear = defaultClear;
    hashMap->exists = defaultExists;
    hashMap->autoAssign = 1;
    // 起始分配8个内存空间, 溢出是会自动扩充
    hashMap->list = (Entry *)malloc(sizeof(Entry) * hashMap->listSize);
    Entry *entry = hashMap->list;
    for (int i = 0; i < hashMap->listSize; i++) {
        entry[i].key = entry[i].value = entry[i].next = NULL;
    }
    return hashMap;
}

// 重新构建
void resetHashMap(HashMap *hashMap, int listSize) {
    if (listSize < 8) {
        return;
    }
    
    // 键值对 临时存储空间
    Entry *tempList = (Entry *)malloc(sizeof(struct entryElement) * hashMap->size);
    
    HashMapIterator *iterator = createHashMapIterator(hashMap);
    int length = hashMap->size;
    
    for (int i = 0; hashNextHashMapIterator(iterator); i++) {
        // 迭代取出所有的键值对
        iterator = nextHashMapIterator(iterator);
        tempList[i].key = iterator->entry->key;
        tempList[i].value = iterator->entry->value;
        tempList[i].next = NULL;
    }
    freeHashMapIterator(iterator);
    
    // 清除原有键值对数据
    hashMap->size = 0;
    for (int i = 0; i < hashMap->listSize; i++) {
        Entry *current = &hashMap->list[i];
        current->key = NULL;
        current->value = NULL;
        if (current->next != NULL) {
            while (current->next != NULL) {
                Entry *temp = current->next->next;
                free(current->next);
                current->next = temp;
            }
        }
    }
    
    // 更改内存大小
    hashMap->listSize = listSize;
    Entry *relist = realloc(hashMap->list, sizeof(struct entryElement) * hashMap->listSize);
    if (relist != NULL) {
        hashMap->list = relist;
        relist = NULL;
    }
    
    // 初始化数据
    for (int i = 0; i < hashMap->listSize; i++) {
        hashMap->list[i].key = NULL;
        hashMap->list[i].value = NULL;
        hashMap->list[i].next = NULL;
    }
    
    // 将所有键值对重新写入内存
    for (int i = 0; i < length; i++) {
        hashMap->put(hashMap, tempList[i].key, tempList[i].value);
    }
    free(tempList);
}

void defaultPut(HashMap *hashMap, void *key, void *value) {
    if (hashMap->autoAssign && hashMap->size >= hashMap->listSize) {
        // 内存扩充至原来的两倍
        resetHashMap(hashMap, hashMap->listSize * 2);
    }
    
    int index = hashMap->hashCode(hashMap, key);
    
    if (hashMap->list[index].key == NULL) {
        hashMap->size++;
        // 该地址为空时 直接存储
        hashMap->list[index].key = key;
        hashMap->list[index].value = value;
    } else {
        Entry *current = &hashMap->list[index];
        while (current != NULL) {
            if (hashMap->equal(key, current->key)) {
                // 对于键值已经存在的直接覆盖
                current->value = value;
                return;
            }
            current = current->next;
        }
        
        // 发生冲突则创建节点挂到相应位置的next上
        Entry *entry = (Entry *)malloc(sizeof(struct entryElement));
        entry->key = key;
        entry->value = value;
        entry->next = hashMap->list[index].next;
        hashMap->list[index].next = entry;
        hashMap->size++;
    }
}

void *defaultGet(HashMap *hashMap, void *key) {
    int index = hashMap->hashCode(hashMap, key);
    Entry *entry = &hashMap->list[index];
    while (entry->key != NULL && !hashMap->equal(entry->key, key)) {
        entry = entry->next;
    }
    return entry->value;
}

int defaultRemove(HashMap *hashMap, void *key) {
    int index = hashMap->hashCode(hashMap, key);
    Entry *entry = &hashMap->list[index];
    if (entry->key == NULL) {
        return 0;
    }
    int result = 0;
    if (hashMap->equal(entry->key, key)) {
        hashMap->size--;
        if (entry->next != NULL) {
            Entry *temp = entry->next;
            entry->key = temp->key;
            entry->value = temp->value;
            entry->next = temp->next;
            free(temp);
        } else {
            entry->key = NULL;
            entry->value = NULL;
        }
        result = 1;
    } else {
        Entry *p = entry;
        entry = entry->next;
        while (entry != NULL) {
            if (hashMap->equal(entry->key, key)) {
                hashMap->size--;
                p->next = entry->next;
                free(entry);
                result = 1;
                break;
            }
            p = entry;
            entry = entry->next;
        }
    }
    
    // 如果空间占用不足一半, 则释放多余的内存
    if (result && hashMap->autoAssign && hashMap->size < hashMap->listSize / 2) {
        resetHashMap(hashMap, hashMap->listSize / 2);
    }
    
    return result;
}

int defaultExists(HashMap *hashMap, void *key) {
    int index = hashMap->hashCode(hashMap, key);
    Entry *entry = &hashMap->list[index];
    if (entry->key == NULL) {
        return 0;
    }
    if (hashMap->equal(entry->key, key)) {
        return 1;
    }
    if (entry->next != NULL) {
        Entry *p = entry->next;
        while (p != NULL) {
            if (hashMap->equal(p->key, key)) {
                return 1;
            }
            p = p->next;
        }
        return 0;
    }
    return 0;
}

void defaultClear(HashMap *hashMap) {
    for (int i = 0; i < hashMap->listSize; i++) {
        // 释放冲突值内存
        Entry *entry = hashMap->list[i].next;
        while (entry != NULL) {
            Entry *temp = entry->next;
            free(entry);
            entry = temp;
        }
        hashMap->list[i].next = NULL;
    }
    free(hashMap->list);
    hashMap->list = NULL;
    hashMap->size = 0;
    hashMap->listSize = 0;
}
