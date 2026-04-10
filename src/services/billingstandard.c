#include "billingstandard.h"
#include "billingdata.h"
#include "utils.h"
#include "linkedlist.h"
#include <stdio.h>
#include <string.h>

static void print_billing_info(Billing* b) {
    showSeparator();
    printf("套餐编号: %s\n", b->sPackageId);
    printf("计费单位: %s\n", b->nUnitType == UNIT_MINUTE ? "分钟" : "小时");
    printf("单价: %.2f 元/%s\n", b->fUnitPrice, b->nUnitType == UNIT_MINUTE ? "分钟" : "小时");
    showSeparator();
}

static void service_query_billing(void) {
    char packageId[PACKAGE_ID_MAX_LEN + 1];
    
    printf("\n查询计费套餐\n");
    int hasInput = readOptionalCardId(packageId, sizeof(packageId));
    
    ListNode* result;
    if (hasInput) {
        result = billing_query(packageId);
    } else {
        result = billing_query_all();
    }
    
    if (result) {
        ListNode* current = result;
        while (current) {
            print_billing_info((Billing*)current->data);
            current = current->next;
        }
        list_free(result);
    } else {
        printf("未找到该套餐。\n");
    }
}

static void service_add_billing(void) {
    Billing billing;
    
    printf("\n新增计费套餐\n");
    printf("套餐编号: ");
    if (scanf("%31s", billing.sPackageId) != 1) {
        clearInputBuffer();
        printf("输入无效。\n");
        return;
    }
    clearInputBuffer();
    billing.sPackageId[sizeof(billing.sPackageId) - 1] = '\0';
    
    printf("计费单位 (0-分钟, 1-小时): ");
    int unitType;
    scanf("%d", &unitType);
    clearInputBuffer();
    billing.nUnitType = (unitType == 1) ? UNIT_HOUR : UNIT_MINUTE;
    
    billing.fUnitPrice = readFloat("请输入单价: ", 0.01f, 9999.99f);
    
    if (billing_add(billing)) {
        printf("套餐添加成功！\n");
    } else {
        printf("套餐添加失败。\n");
    }
}

static void service_update_billing(void) {
    char packageId[PACKAGE_ID_MAX_LEN + 1];
    
    printf("\n修改计费套餐\n");
    printf("套餐编号: ");
    if (scanf("%31s", packageId) != 1) {
        clearInputBuffer();
        printf("输入无效。\n");
        return;
    }
    clearInputBuffer();
    
    ListNode* result = billing_query(packageId);
    if (!result) {
        printf("未找到该套餐。\n");
        return;
    }
    
    Billing billing = *((Billing*)result->data);
    list_free(result);
    
    print_billing_info(&billing);
    
    printf("是否修改计费单位？(y/n): ");
    char choice = readOption("");
    if (choice == 'y' || choice == 'Y') {
        printf("新计费单位 (0-分钟, 1-小时): ");
        int unitType;
        scanf("%d", &unitType);
        clearInputBuffer();
        billing.nUnitType = (unitType == 1) ? UNIT_HOUR : UNIT_MINUTE;
    }
    
    printf("是否修改单价？(y/n): ");
    choice = readOption("");
    if (choice == 'y' || choice == 'Y') {
        billing.fUnitPrice = readFloat("新单价: ", 0.01f, 9999.99f);
    }
    
    if (billing_update(billing)) {
        printf("套餐更新成功！\n");
    } else {
        printf("套餐更新失败。\n");
    }
}

static void service_delete_billing(void) {
    char packageId[PACKAGE_ID_MAX_LEN + 1];
    
    printf("\n删除计费套餐\n");
    printf("套餐编号: ");
    if (scanf("%31s", packageId) != 1) {
        clearInputBuffer();
        printf("输入无效。\n");
        return;
    }
    clearInputBuffer();
    
    printf("确认删除该套餐？(y/n): ");
    char choice = readOption("");
    
    if (choice == 'y' || choice == 'Y') {
        if (billing_delete(packageId)) {
            printf("套餐删除成功！\n");
        } else {
            printf("套餐删除失败。\n");
        }
    } else {
        printf("已取消操作。\n");
    }
}

void billing_standard_service(void) {
    const char* options[] = {
        "查询套餐",
        "新增套餐",
        "修改套餐",
        "删除套餐",
        "返回上级"
    };
    
    while (1) {
        int choice = showMenu("计费标准管理", options, sizeof(options)/sizeof(options[0]));
        
        switch (choice) {
            case 0: service_query_billing(); break;
            case 1: service_add_billing(); break;
            case 2: service_update_billing(); break;
            case 3: service_delete_billing(); break;
            case 4: return;
        }
    }
}
