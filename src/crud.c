#include "crud.h"
#include "card.h"
#include "finance.h"
#include "fileio.h"
#include "utils.h"
#include <stdio.h>
#include <time.h>

void crud_menu(void) {
    while (1) {
        printf("\n=== 增删改查 ===\n");
        printf("1. 查询所有卡\n");
        printf("2. 查询上下机记录\n");
        printf("3. 查询财务记录\n");
        printf("0. 返回\n");
        
        int choice = get_choice(0, 3);
        if (choice == 0) break;
        
        switch (choice) {
            case 1:
                crud_all_cards();
                break;
            case 2:
                crud_all_logs();
                break;
            case 3:
                crud_finance_records();
                break;
        }
    }
}

void crud_all_cards(void) {
    printf("\n=== 所有卡信息 ===\n");
    AccountNode* p = card_get_head();
    int count = 0;
    
    while (p) {
        if (p->data.nDel == 0) {
            printf("卡号: %s, 状态: %s, 余额: %.2f, 累计消费: %.2f\n",
                   p->data.aName,
                   p->data.nStatus[0] == '0' ? "未上机" : 
                       p->data.nStatus[0] == '1' ? "上机中" : "已注销",
                   p->data.fBalance,
                   p->data.fTotalUse);
            count++;
        }
        p = p->next;
    }
    printf("共 %d 张卡\n", count);
}

void crud_all_logs(void) {
    printf("\n=== 上下机记录 ===\n");
    LogInfoNode* p = load_loginfo();
    int count = 0;
    
    while (p) {
        printf("卡号: %s, 消费: %.2f, 余额: %.2f\n",
               p->data.aCardName,
               p->data.fAmount,
               p->data.fBalance);
        count++;
        p = p->next;
    }
    printf("共 %d 条记录\n", count);
}

void crud_finance_records(void) {
    printf("\n=== 财务记录 ===\n");
    FinanceNode* p = finance_get_head();
    int count = 0;
    
    while (p) {
        printf("卡号: %s, 类型: %s, 金额: %.2f, 余额: %.2f\n",
               p->data.cardId,
               p->data.type == 1 ? "充值" : (p->data.type == 2 ? "退费" : "消费"),
               p->data.amount,
               p->data.balanceAfter);
        count++;
        p = p->next;
    }
    printf("共 %d 条记录\n", count);
}
