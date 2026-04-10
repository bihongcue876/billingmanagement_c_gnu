#include "accountservice.h"
#include "accountdata.h"
#include "utils.h"
#include "linkedlist.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

static void print_account_info(Account* acc) {
    char timeBuf[64];
    
    showSeparator();
    printf("卡号: %s\n", acc->aName);
    printf("状态: ");
    switch (acc->nStatus[0]) {
        case '0': printf("未上机\n"); break;
        case '1': printf("上机中\n"); break;
        case '2': printf("已注销\n"); break;
        case '3': printf("失效\n"); break;
        default: printf("未知\n"); break;
    }
    printf("余额: %.2f 元\n", acc->fBalance);
    printf("累计消费: %.2f 元\n", acc->fTotalUse);
    printf("使用次数: %d\n", acc->nUseCount);
    
    formatTime(acc->tStart, timeBuf, sizeof(timeBuf));
    printf("开卡时间: %s\n", timeBuf);
    
    if (acc->tLast > 0) {
        formatTime(acc->tLast, timeBuf, sizeof(timeBuf));
        printf("最后使用时间: %s\n", timeBuf);
    }
    showSeparator();
}

static void service_query_account(void) {
    char cardId[CARD_ID_MAX_LEN + 1];
    
    printf("\n查询账户\n");
    readCardId(cardId, sizeof(cardId));
    
    ListNode* result = account_query(cardId);
    if (result) {
        print_account_info((Account*)result->data);
        list_free(result);
    } else {
        printf("未找到该账户。\n");
    }
}

static void service_list_accounts(void) {
    ListNode* head = account_query_all();
    if (!head) {
        printf("暂无有效账户。\n");
        return;
    }
    
    printf("\n-----------------------------------\n");
    printf("所有有效账户列表\n");
    printf("-----------------------------------\n");
    
    float totalBalance = 0.0f;
    int count = 0;
    ListNode* current = head;
    
    while (current) {
        print_account_info((Account*)current->data);
        totalBalance += ((Account*)current->data)->fBalance;
        count++;
        current = current->next;
    }
    
    printf("\n总计: %d 个账户，总余额: %.2f 元\n", count, totalBalance);
    list_free(head);
}

static void service_register_account(void) {
    Account account;
    char cardId[CARD_ID_MAX_LEN + 1];
    char password[PASSWORD_MAX_LEN + 1];
    float balance;
    
    printf("\n注册新账户\n");
    readCardId(cardId, sizeof(cardId));
    strncpy(account.aName, cardId, sizeof(account.aName) - 1);
    account.aName[sizeof(account.aName) - 1] = '\0';
    
    printf("设置密码: ");
    readPassword(password, sizeof(password));
    strncpy(account.aPwd, password, sizeof(account.aPwd) - 1);
    account.aPwd[sizeof(account.aPwd) - 1] = '\0';
    
    balance = readFloat("请输入充值金额: ", 0.0f, MAX_INITIAL_BALANCE);
    account.fBalance = balance;
    
    if (account_register(account)) {
        printf("账户注册成功！\n");
    } else {
        printf("账户注册失败。\n");
    }
}

static void service_update_account(void) {
    char cardId[CARD_ID_MAX_LEN + 1];
    
    printf("\n修改账户信息\n");
    readCardId(cardId, sizeof(cardId));
    
    ListNode* result = account_query(cardId);
    if (!result) {
        printf("未找到该账户。\n");
        return;
    }
    
    Account acc = *((Account*)result->data);
    list_free(result);
    
    if (acc.nDel == 1) {
        printf("该账户已注销，无法修改。\n");
        return;
    }
    
    printf("当前状态: ");
    switch (acc.nStatus[0]) {
        case '0': printf("未上机\n"); break;
        case '1': printf("上机中\n"); break;
        case '2': printf("已注销\n"); break;
        default: printf("未知\n"); break;
    }
    
    char newPwd[PASSWORD_MAX_LEN + 1];
    printf("是否修改密码？(y/n): ");
    char choice = readOption("");
    if (choice == 'y' || choice == 'Y') {
        printf("新密码: ");
        readPassword(newPwd, sizeof(newPwd));
        strncpy(acc.aPwd, newPwd, sizeof(acc.aPwd) - 1);
        acc.aPwd[sizeof(acc.aPwd) - 1] = '\0';
    }
    
    printf("是否修改状态？(y/n): ");
    choice = readOption("");
    if (choice == 'y' || choice == 'Y') {
        printf("新状态 (0-未上机, 2-已注销): ");
        char status[2];
        scanf("%1s", status);
        clearInputBuffer();
        strncpy(acc.nStatus, status, sizeof(acc.nStatus) - 1);
        acc.nStatus[sizeof(acc.nStatus) - 1] = '\0';
    }
    
    if (account_update(acc)) {
        printf("账户更新成功！\n");
    } else {
        printf("账户更新失败。\n");
    }
}

static void service_delete_account(void) {
    char cardId[CARD_ID_MAX_LEN + 1];
    
    printf("\n注销账户\n");
    readCardId(cardId, sizeof(cardId));
    
    ListNode* result = account_query(cardId);
    if (!result) {
        printf("未找到该账户。\n");
        return;
    }
    
    print_account_info((Account*)result->data);
    list_free(result);
    
    printf("确认注销该账户？(y/n): ");
    char choice = readOption("");
    
    if (choice == 'y' || choice == 'Y') {
        if (account_deactivate(cardId)) {
            printf("账户注销成功！\n");
        } else {
            printf("账户注销失败。\n");
        }
    } else {
        printf("已取消操作。\n");
    }
}

static void service_reactivate_account(void) {
    char cardId[CARD_ID_MAX_LEN + 1];
    
    printf("\n重新启用账户\n");
    readCardId(cardId, sizeof(cardId));
    
    if (account_reactivate(cardId)) {
        printf("账户重新启用成功！\n");
    } else {
        printf("账户重新启用失败。\n");
    }
}

void account_service(void) {
    const char* options[] = {
        "查询账户",
        "账户列表",
        "注册账户",
        "修改账户",
        "注销账户",
        "重新启用账户",
        "返回上级"
    };
    
    while (1) {
        int choice = showMenu("账户服务", options, sizeof(options)/sizeof(options[0]));
        
        switch (choice) {
            case 0: service_query_account(); break;
            case 1: service_list_accounts(); break;
            case 2: service_register_account(); break;
            case 3: service_update_account(); break;
            case 4: service_delete_account(); break;
            case 5: service_reactivate_account(); break;
            case 6: return;
        }
    }
}
