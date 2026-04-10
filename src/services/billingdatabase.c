#include "billingdata.h"
#include "fileio.h"
#include "linkedlist.h"
#include <stdio.h>
#include <string.h>
#include <stddef.h>

BillingNode* billing_query(const char* packageId) {
    ListNode* head = file_read_all(BILLING_FILE, FILE_MAGIC_BILLING, sizeof(Billing));
    if (!head) return NULL;
    
    ListNode* found = list_find_by_key(head, packageId, offsetof(Billing, sPackageId), PACKAGE_ID_MAX_LEN + 1);
    if (!found || ((Billing*)found->data)->nDel == 1) {
        list_free(head);
        return NULL;
    }
    
    BillingNode* result = list_append(NULL, found->data, sizeof(Billing));
    list_free(head);
    return result;
}

BillingNode* billing_query_all(void) {
    ListNode* head = file_read_all(BILLING_FILE, FILE_MAGIC_BILLING, sizeof(Billing));
    if (!head) return NULL;
    
    ListNode* result = NULL;
    ListNode* current = head;
    
    while (current) {
        Billing* b = (Billing*)current->data;
        if (b->nDel == 0) {
            result = list_append(result, current->data, sizeof(Billing));
        }
        current = current->next;
    }
    
    list_free(head);
    return result;
}

int billing_add(Billing billing) {
    /* 检查套餐编号是否已存在 */
    ListNode* head = file_read_all(BILLING_FILE, FILE_MAGIC_BILLING, sizeof(Billing));
    if (head) {
        ListNode* found = list_find_by_key(head, billing.sPackageId, offsetof(Billing, sPackageId), PACKAGE_ID_MAX_LEN + 1);
        if (found && ((Billing*)found->data)->nDel == 0) {
            printf("该套餐编号已存在。\n");
            list_free(head);
            return 0;
        }
        list_free(head);
    }
    
    billing.nDel = 0;
    return file_append_record(BILLING_FILE, FILE_MAGIC_BILLING, &billing, sizeof(Billing));
}

int billing_update(Billing billing) {
    return file_update_record(BILLING_FILE, FILE_MAGIC_BILLING, &billing, sizeof(Billing),
                              offsetof(Billing, sPackageId), PACKAGE_ID_MAX_LEN + 1);
}

int billing_delete(const char* packageId) {
    /* 不允许删除默认套餐 */
    if (strcmp(packageId, DEFAULT_PACKAGE_ID) == 0) {
        printf("默认套餐不可删除。\n");
        return 0;
    }
    
    ListNode* head = file_read_all(BILLING_FILE, FILE_MAGIC_BILLING, sizeof(Billing));
    if (!head) return 0;
    
    ListNode* found = list_find_by_key(head, packageId, offsetof(Billing, sPackageId), PACKAGE_ID_MAX_LEN + 1);
    if (!found) {
        printf("未找到该套餐。\n");
        list_free(head);
        return 0;
    }
    
    if (((Billing*)found->data)->nDel == 1) {
        printf("该套餐已删除。\n");
        list_free(head);
        return 0;
    }
    
    ((Billing*)found->data)->nDel = 1;
    
    int result = file_write_all(BILLING_FILE, FILE_MAGIC_BILLING, head, sizeof(Billing));
    list_free(head);
    return result;
}

Billing billing_get_default(void) {
    Billing defaultBilling;
    strncpy(defaultBilling.sPackageId, DEFAULT_PACKAGE_ID, sizeof(defaultBilling.sPackageId) - 1);
    defaultBilling.sPackageId[sizeof(defaultBilling.sPackageId) - 1] = '\0';
    defaultBilling.nUnitType = DEFAULT_UNIT_TYPE;
    defaultBilling.fUnitPrice = DEFAULT_UNIT_PRICE;
    defaultBilling.nDel = 0;
    return defaultBilling;
}
