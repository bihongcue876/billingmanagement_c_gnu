/**
 * @file linkedlist.c
 * @brief 通用链表实现 - 使用void*存储数据
 * 
 * 设计思路：
 * - 节点存储void*指针，数据在创建时按指定大小分配
 * - 所有链表操作（create/free/append/find/remove）统一实现
 * - 通过offsetof和memcmp实现通用键值查找
 */

#include "linkedlist.h"
#include <string.h>
#include <stdio.h>

ListNode* list_node_create(size_t dataSize) {
    ListNode* node = (ListNode*)malloc(sizeof(ListNode));
    if (!node) return NULL;
    
    node->data = malloc(dataSize);
    if (!node->data) {
        free(node);
        return NULL;
    }
    
    memset(node->data, 0, dataSize);
    node->next = NULL;
    return node;
}

void list_node_free(ListNode* node) {
    if (node) {
        free(node->data);
        free(node);
    }
}

void list_free(ListNode* head) {
    ListNode* current = head;
    while (current) {
        ListNode* next = current->next;
        list_node_free(current);
        current = next;
    }
}

int list_size(ListNode* head) {
    int count = 0;
    ListNode* current = head;
    while (current) {
        count++;
        current = current->next;
    }
    return count;
}

ListNode* list_append(ListNode* head, const void* data, size_t dataSize) {
    ListNode* newNode = list_node_create(dataSize);
    if (!newNode) return head;
    
    memcpy(newNode->data, data, dataSize);
    
    if (!head) {
        return newNode;
    }
    
    ListNode* current = head;
    while (current->next) {
        current = current->next;
    }
    current->next = newNode;
    return head;
}

/* 通过字符串键值查找节点，使用strcmp比较 */
ListNode* list_find_by_key(ListNode* head, const char* key, size_t keyOffset, size_t keySize) {
    (void)keySize;  /* 未使用，保留接口兼容 */
    ListNode* current = head;
    while (current) {
        char* structPtr = (char*)current->data;
        if (strcmp(structPtr + keyOffset, key) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

/* 通过字符串键值删除节点 */
ListNode* list_remove_by_key(ListNode* head, const char* key, size_t keyOffset, size_t keySize) {
    (void)keySize;
    if (!head) return NULL;

    char* headData = (char*)head->data;
    if (strcmp(headData + keyOffset, key) == 0) {
        ListNode* newHead = head->next;
        list_node_free(head);
        return newHead;
    }

    ListNode* current = head;
    while (current->next) {
        char* nextData = (char*)current->next->data;
        if (strcmp(nextData + keyOffset, key) == 0) {
            ListNode* toRemove = current->next;
            current->next = toRemove->next;
            list_node_free(toRemove);
            break;
        }
        current = current->next;
    }
    return head;
}
