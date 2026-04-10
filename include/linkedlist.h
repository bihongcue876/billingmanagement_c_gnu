#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <stdlib.h>

/* 通用链表 - 使用void*存储数据，减少重复代码 */

typedef struct ListNode {
    void* data;              /* 数据指针（由调用者分配/释放） */
    struct ListNode* next;   /* 下一个节点 */
} ListNode;

/* 基础链表操作 */
ListNode* list_node_create(size_t dataSize);
void list_node_free(ListNode* node);
void list_free(ListNode* head);
int list_size(ListNode* head);
ListNode* list_append(ListNode* head, const void* data, size_t dataSize);
ListNode* list_find_by_key(ListNode* head, const char* key, size_t keyOffset, size_t keySize);
ListNode* list_remove_by_key(ListNode* head, const char* key, size_t keyOffset, size_t keySize);

/* 类型安全宏包装 */
#include "model.h"

#define ACCOUNT_NODE(ptr) ((AccountNode*)(ptr))
#define ACCOUNT_DATA(ptr) (&((AccountNode*)(ptr))->data)
#define BILLING_NODE(ptr) ((BillingNode*)(ptr))
#define BILLING_DATA(ptr) (&((BillingNode*)(ptr))->data)
#define LOGIN_NODE(ptr) ((LogInfoNode*)(ptr))
#define LOGIN_DATA(ptr) (&((LogInfoNode*)(ptr))->data)
#define FINANCE_NODE(ptr) ((FinanceNode*)(ptr))
#define FINANCE_DATA(ptr) (&((FinanceNode*)(ptr))->data)
#define ADMIN_NODE(ptr) ((AdminNode*)(ptr))
#define ADMIN_DATA(ptr) (&((AdminNode*)(ptr))->data)

#endif /* LINKEDLIST_H */
