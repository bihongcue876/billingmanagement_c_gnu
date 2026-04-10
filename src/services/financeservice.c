#include "financeservice.h"
#include "financedata.h"
#include "accountdata.h"
#include "utils.h"
#include "linkedlist.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

static void print_finance_info(Finance* f) {
    showSeparator();
    printf("交易ID: %d\n", f->id);
    printf("卡号: %s\n", f->cardId);
    printf("类型: ");
    switch (f->type) {
        case 1: printf("充值\n"); break;
        case 2: printf("退费\n"); break;
        case 3: printf("消费\n"); break;
        default: printf("未知\n"); break;
    }
    printf("金额: %.2f 元\n", f->amount);
    printf("交易前余额: %.2f 元\n", f->balanceBefore);
    printf("交易后余额: %.2f 元\n", f->balanceAfter);
    printf("备注: %s\n", f->remark);
    printf("时间: %s\n", f->time);
    showSeparator();
}

static void service_recharge(void) {
    char cardId[CARD_ID_MAX_LEN + 1];
    float amount;
    
    printf("\n账户充值\n");
    readCardId(cardId, sizeof(cardId));
    
    ListNode* result = account_query(cardId);
    if (!result) {
        printf("未找到该账户。\n");
        return;
    }
    
    Account account = *((Account*)result->data);
    list_free(result);
    
    amount = readFloat("请输入充值金额: ", 0.01f, MAX_RECHARGE_AMOUNT);
    
    /* 大额确认 */
    if (amount > LARGE_AMOUNT_THRESHOLD) {
        printf("充值金额较大 (%.2f 元)，确认充值？(y/n): ", amount);
        char choice = readOption("");
        if (choice != 'y' && choice != 'Y') {
            printf("已取消操作。\n");
            return;
        }
    }
    
    float balanceBefore = account.fBalance;
    account.fBalance += amount;
    float balanceAfter = account.fBalance;
    
    /* 更新账户 */
    account_update(account);
    
    /* 记录财务 */
    Finance finance;
    strncpy(finance.cardId, cardId, sizeof(finance.cardId) - 1);
    finance.cardId[sizeof(finance.cardId) - 1] = '\0';
    finance.type = 1;  /* 充值 */
    finance.amount = amount;
    finance.balanceBefore = balanceBefore;
    finance.balanceAfter = balanceAfter;
    strncpy(finance.remark, "账户充值", sizeof(finance.remark) - 1);
    finance.remark[sizeof(finance.remark) - 1] = '\0';
    
    finance_add(finance);
    
    printf("充值成功！当前余额: %.2f 元\n", balanceAfter);
}

static void service_refund(void) {
    char cardId[CARD_ID_MAX_LEN + 1];
    float amount;
    
    printf("\n账户退费\n");
    readCardId(cardId, sizeof(cardId));
    
    ListNode* result = account_query(cardId);
    if (!result) {
        printf("未找到该账户。\n");
        return;
    }
    
    Account account = *((Account*)result->data);
    list_free(result);
    
    printf("当前余额: %.2f 元\n", account.fBalance);
    amount = readFloat("请输入退费金额: ", 0.01f, account.fBalance);
    
    /* 大额确认 */
    if (amount > LARGE_AMOUNT_THRESHOLD) {
        printf("退费金额较大 (%.2f 元)，确认退费？(y/n): ", amount);
        char choice = readOption("");
        if (choice != 'y' && choice != 'Y') {
            printf("已取消操作。\n");
            return;
        }
    }
    
    float balanceBefore = account.fBalance;
    account.fBalance -= amount;
    float balanceAfter = account.fBalance;
    
    /* 更新账户 */
    account_update(account);
    
    /* 记录财务 */
    Finance finance;
    strncpy(finance.cardId, cardId, sizeof(finance.cardId) - 1);
    finance.cardId[sizeof(finance.cardId) - 1] = '\0';
    finance.type = 2;  /* 退费 */
    finance.amount = amount;
    finance.balanceBefore = balanceBefore;
    finance.balanceAfter = balanceAfter;
    strncpy(finance.remark, "账户退费", sizeof(finance.remark) - 1);
    finance.remark[sizeof(finance.remark) - 1] = '\0';
    
    finance_add(finance);
    
    printf("退费成功！当前余额: %.2f 元\n", balanceAfter);
}

static void service_query_finance(void) {
    char cardId[CARD_ID_MAX_LEN + 1];
    
    printf("\n消费记录查询\n");
    int hasCardId = readOptionalCardId(cardId, sizeof(cardId));
    
    ListNode* head;
    if (hasCardId) {
        head = finance_query_by_card(cardId);
        if (!head) {
            printf("暂无该卡的记录。\n");
            return;
        }
    } else {
        head = finance_query_all();
        if (!head) {
            printf("暂无记录。\n");
            return;
        }
    }
    
    ListNode* current = head;
    while (current) {
        print_finance_info((Finance*)current->data);
        current = current->next;
    }
    list_free(head);
}

static void service_statistics(void) {
    float recharge = finance_statistics(1);
    float refund = finance_statistics(2);
    float consume = finance_statistics(3);
    
    printf("\n-----------------------------------\n");
    printf("营业额统计\n");
    printf("-----------------------------------\n");
    printf("充值总额: %.2f 元\n", recharge);
    printf("退费总额: %.2f 元\n", refund);
    printf("消费总额: %.2f 元\n", consume);
    printf("净营业额: %.2f 元\n", recharge - refund);
    showSeparator();
}

static void service_query_by_date(void) {
    int year, month, day;
    
    printf("\n按日期查询交易\n");
    printf("请输入日期 (YYYY MM DD): ");
    scanf("%d %d %d", &year, &month, &day);
    clearInputBuffer();
    
    ListNode* head = finance_query_by_date(year, month, day);
    if (!head) {
        printf("该日期暂无记录。\n");
        return;
    }
    
    ListNode* current = head;
    while (current) {
        print_finance_info((Finance*)current->data);
        current = current->next;
    }
    list_free(head);
}

void finance_service(void) {
    const char* options[] = {
        "账户充值",
        "账户退费",
        "消费记录查询",
        "营业额统计",
        "按日期查询交易",
        "返回上级"
    };
    
    while (1) {
        int choice = showMenu("财务服务", options, sizeof(options)/sizeof(options[0]));
        
        switch (choice) {
            case 0: service_recharge(); break;
            case 1: service_refund(); break;
            case 2: service_query_finance(); break;
            case 3: service_statistics(); break;
            case 4: service_query_by_date(); break;
            case 5: return;
        }
    }
}
