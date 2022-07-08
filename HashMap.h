//
//  HashMap.h
//  HashMap实现
//
//  Created by zhaofei on 2022/7/8.
//

#ifndef HashMap_h
#define HashMap_h

#include <stdio.h>

// 键值对结构
typedef struct entryElement {
    void *key;                  // 键
    void *value;                // 值
    struct entryElement *next;  // 冲突链表
} Entry;

// 哈希结构
typedef struct hashMap HashMap;

// 哈希函数类型
typedef int (*HashCode)(HashMap *hashMap, void *key);
// 判等函数类型
typedef int (*Equal)(void *key1, void *key2);
// 添加键函数类型
typedef void(*Put)(HashMap *hasMap, void *key, void *value);
// 获取键对应值的函数类型
typedef void* (*Get)(HashMap *hashMap, void *key);
// 删除键的函数类型
typedef int (*Remove)(HashMap *hashMap, void *key);
// 清空Map的函数类型
typedef void (*Clear)(HashMap *hashMap);
// 判断键值是否存在的函数类型
typedef int (*Exists)(HashMap *hashMap, void *key);


typedef struct hashMap {
    int         size;               // 当前大小
    int         listSize;           // 有效空间
    HashCode    hashCode;           // 哈希函数
    Equal       equal;              // 判等函数
    Entry       *list;               // 存储区域
    Put         put;                // 添加键值对的函数
    Get         get;                // 获取键对应值的函数
    Remove      remove;             // 删除键的函数
    Clear       clear;              // 清空Map的函数
    Exists      exists;             // 判断键值是否存在的函数
    int        autoAssign;          // 设定是否根据当前数据量动态调整内存大小, 1开启, 0关闭 默认开启
} HashMap;


// 创建一个哈希表
HashMap *createHashMap(HashCode hashCode, Equal equal);


// 迭代器
typedef struct hashMapIterator {
    Entry   *entry;         // 迭代器当前指向
    int     count;          // 迭代次数
    int     hashCode;       // 键值对的哈希值
    HashMap *hashMap;
} HashMapIterator;

// 创建哈希结构迭代器
HashMapIterator *createHashMapIterator(HashMap *hashMap);
// 哈希结构迭代器 是否有下一个
int hashNextHashMapIterator(HashMapIterator *iterator);
// 迭代到下一次
HashMapIterator *nextHashMapIterator(HashMapIterator *iterator);
void freeHashMapIterator(HashMapIterator *iterator);

#endif /* HashMap_h */
