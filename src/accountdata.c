#include "accountdata.h"
#include "fileio.h"
#include "linkedlist.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stddef.h>

AccountNode* account_query(const char* cardId) {
    ListNode* head = file_read_all(ACCOUNT_FILE, FILE_MAGIC_ACCOUNT, sizeof(Account));
    if (!head) return NULL;
    
    ListNode* found = list_find_by_key(head, cardId, offsetof(Account, aName), CARD_ID_MAX_LEN + 1);
    if (!found) {
        list_free(head);
        return NULL;
    }
    
    /* 创建只包含该账户的新链表 */
    AccountNode* result = list_append(NULL, found->data, sizeof(Account));
    list_free(head);
    return result;
}

AccountNode* account_query_all(void) {
    ListNode* head = file_read_all(ACCOUNT_FILE, FILE_MAGIC_ACCOUNT, sizeof(Account));
    if (!head) return NULL;
    
    /* 过滤已删除的账户 */
    ListNode* result = NULL;
    ListNode* current = head;
    
    while (current) {
        Account* acc = (Account*)current->data;
        if (acc->nDel == 0) {
            result = list_append(result, current->data, sizeof(Account));
        }
        current = current->next;
    }
    
    list_free(head);
    return result;
}

int account_register(Account account) {
    /* 检查卡号是否已存在 */
    ListNode* head = file_read_all(ACCOUNT_FILE, FILE_MAGIC_ACCOUNT, sizeof(Account));
    if (head) {
        ListNode* found = list_find_by_key(head, account.aName, offsetof(Account, aName), CARD_ID_MAX_LEN + 1);
        if (found) {
            printf("该卡号已存在，无法重复注册。\n");
            list_free(head);
            return 0;
        }
        list_free(head);
    }
    
    /* 设置默认值 */
    account.nStatus[0] = '0';
    account.nStatus[1] = '\0';
    account.tStart = time(NULL);
    account.tEnd = 0;
    account.fTotalUse = 0.0f;
    account.tLast = 0;
    account.nUseCount = 0;
    account.nDel = 0;
    
    return file_append_record(ACCOUNT_FILE, FILE_MAGIC_ACCOUNT, &account, sizeof(Account));
}

int account_deactivate(const char* cardId) {
    ListNode* head = file_read_all(ACCOUNT_FILE, FILE_MAGIC_ACCOUNT, sizeof(Account));
    if (!head) return 0;
    
    ListNode* found = list_find_by_key(head, cardId, offsetof(Account, aName), CARD_ID_MAX_LEN + 1);
    if (!found) {
        printf("未找到该卡号。\n");
        list_free(head);
        return 0;
    }
    
    Account* acc = (Account*)found->data;
    if (acc->nDel == 1) {
        printf("该账户已注销。\n");
        list_free(head);
        return 0;
    }
    
    acc->nDel = 1;
    acc->nStatus[0] = '2';
    acc->nStatus[1] = '\0';
    
    int result = file_write_all(ACCOUNT_FILE, FILE_MAGIC_ACCOUNT, head, sizeof(Account));
    list_free(head);
    return result;
}

int account_reactivate(const char* cardId) {
    ListNode* head = file_read_all(ACCOUNT_FILE, FILE_MAGIC_ACCOUNT, sizeof(Account));
    if (!head) return 0;
    
    ListNode* found = list_find_by_key(head, cardId, offsetof(Account, aName), CARD_ID_MAX_LEN + 1);
    if (!found) {
        printf("未找到该卡号。\n");
        list_free(head);
        return 0;
    }
    
    Account* acc = (Account*)found->data;
    if (acc->nDel == 0) {
        printf("该账户未被注销。\n");
        list_free(head);
        return 0;
    }
    
    acc->nDel = 0;
    acc->nStatus[0] = '0';
    acc->nStatus[1] = '\0';
    
    int result = file_write_all(ACCOUNT_FILE, FILE_MAGIC_ACCOUNT, head, sizeof(Account));
    list_free(head);
    return result;
}

int account_update(Account account) {
    return file_update_record(ACCOUNT_FILE, FILE_MAGIC_ACCOUNT, &account, sizeof(Account),
                              offsetof(Account, aName), CARD_ID_MAX_LEN + 1);
}

float account_get_total_balance(void) {
    ListNode* head = account_query_all();
    if (!head) return 0.0f;
    
    float total = 0.0f;
    ListNode* current = head;
    
    while (current) {
        total += ((Account*)current->data)->fBalance;
        current = current->next;
    }
    
    list_free(head);
    return total;
}
