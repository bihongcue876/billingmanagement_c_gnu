#include "finance.h"
#include "card.h"
#include "fileio.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static FinanceNode* finance_head = NULL;

void finance_set_head(FinanceNode* h) {
    finance_head = h;
}

FinanceNode* finance_get_head(void) {
    return finance_head;
}

static int get_next_id(void) {
    int max_id = 0;
    FinanceNode* p = finance_head;
    while (p) {
        if (p->data.id > max_id) {
            max_id = p->data.id;
        }
        p = p->next;
    }
    return max_id + 1;
}

int recharge(const char* cardId, float amount) {
    AccountNode* p = card_find(cardId);
    if (!p) {
        printf("卡号不存在！\n");
        return 0;
    }
    
    if (amount <= 0) {
        printf("充值金额必须大于0！\n");
        return 0;
    }
    
    float old_balance = p->data.fBalance;
    p->data.fBalance += amount;
    
    FinanceNode* node = (FinanceNode*)malloc(sizeof(FinanceNode));
    node->data.id = get_next_id();
    strcpy(node->data.cardId, cardId);
    node->data.type = 1;
    node->data.amount = amount;
    node->data.balanceBefore = old_balance;
    node->data.balanceAfter = p->data.fBalance;
    strcpy(node->data.remark, "充值");
    sprintf(node->data.time, "%ld", (long)time(NULL));
    node->next = NULL;
    
    if (!finance_head) {
        finance_head = node;
    } else {
        FinanceNode* tail = finance_head;
        while (tail->next) tail = tail->next;
        tail->next = node;
    }
    
    save_finance(finance_head);
    printf("充值成功！卡号: %s，充值金额: %.2f，当前余额: %.2f\n", 
           cardId, amount, p->data.fBalance);
    return 1;
}

int refund(const char* cardId, float amount) {
    AccountNode* p = card_find(cardId);
    if (!p) {
        printf("卡号不存在！\n");
        return 0;
    }
    
    if (amount <= 0) {
        printf("退费金额必须大于0！\n");
        return 0;
    }
    
    if (amount > p->data.fBalance) {
        printf("余额不足！当前余额: %.2f\n", p->data.fBalance);
        return 0;
    }
    
    float old_balance = p->data.fBalance;
    p->data.fBalance -= amount;
    
    FinanceNode* node = (FinanceNode*)malloc(sizeof(FinanceNode));
    node->data.id = get_next_id();
    strcpy(node->data.cardId, cardId);
    node->data.type = 2;
    node->data.amount = amount;
    node->data.balanceBefore = old_balance;
    node->data.balanceAfter = p->data.fBalance;
    strcpy(node->data.remark, "退费");
    sprintf(node->data.time, "%ld", (long)time(NULL));
    node->next = NULL;
    
    if (!finance_head) {
        finance_head = node;
    } else {
        FinanceNode* tail = finance_head;
        while (tail->next) tail = tail->next;
        tail->next = node;
    }
    
    save_finance(finance_head);
    printf("退费成功！卡号: %s，退费金额: %.2f，当前余额: %.2f\n", 
           cardId, amount, p->data.fBalance);
    return 1;
}
