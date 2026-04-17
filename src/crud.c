#include "crud.h"
#include "card.h"
#include "finance.h"
#include "fileio.h"
#include "utils.h"
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

void crud_menu(void) {
    while (1) {
        printf("\n=== 增删改查 ===\n");
        printf("1. 查询所有卡\n");
        printf("2. 查询上下机记录\n");
        printf("3. 查询营业额记录\n");
        printf("4. 用户全记录\n");
        printf("5. 详细记录\n");
        printf("0. 返回\n");
        
        int choice = get_choice(0, 5);
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
            case 4:
                crud_user_logs();
                break;
            case 5:
                crud_user_detail();
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
    
    char result[32];
    sprintf(result, "Found %d cards", count);
    log_query("QueryAllCards", "ALL", result);
}

void crud_all_logs(void) {
    printf("\n=== 上下机记录 ===\n");
    LogInfoNode* p = load_loginfo();
    int count = 0;
    
    time_t now = time(NULL);
    struct tm* t = localtime(&now);
    int year = t->tm_year + 1900;
    char logFile[64];
    sprintf(logFile, "uselog%d.log", year);
    FILE* fp = fopen(logFile, "a");
    if (fp) {
        fprintf(fp, "\n=== UseLog Query ===\n");
    }
    
    while (p) {
        char line[128];
        sprintf(line, "Card: %s, Amount: %.2f, Balance: %.2f\n",
               p->data.aCardName,
               p->data.fAmount,
               p->data.fBalance);
        printf("%s", line);
        if (fp) fprintf(fp, "%s", line);
        count++;
        p = p->next;
    }
    printf("共 %d 条记录\n", count);
    if (fp) {
        fprintf(fp, "Total: %d records\n", count);
        fclose(fp);
    }
    
    char result[32];
    sprintf(result, "Found %d records", count);
    log_query("UseLog", "ALL", result);
}

void crud_finance_records(void) {
    printf("\n=== 财务记录 ===\n");
    FinanceNode* p = finance_get_head();
    int count = 0;
    
    time_t now = time(NULL);
    struct tm* t = localtime(&now);
    int year = t->tm_year + 1900;
    char logFile[64];
    sprintf(logFile, "sales%d.log", year);
    FILE* fp = fopen(logFile, "a");
    if (fp) {
        fprintf(fp, "\n=== Sales Query ===\n");
    }
    
    while (p) {
        char timeStr[32] = "";
        if (strlen(p->data.time) > 0) {
            time_t tt = atol(p->data.time);
            struct tm* timeinfo = localtime(&tt);
            strftime(timeStr, sizeof(timeStr), "%Y/%m/%d %H:%M:%S", timeinfo);
        }
        char typeStr[16];
        if (p->data.type == 1) strcpy(typeStr, "Recharge");
        else if (p->data.type == 2) strcpy(typeStr, "Refund");
        else strcpy(typeStr, "Consume");
        
        char line[128];
        sprintf(line, "Card: %s, Type: %s, Amount: %.2f, Balance: %.2f, Time: %s\n",
               p->data.cardId,
               typeStr,
               p->data.amount,
               p->data.balanceAfter,
               timeStr);
        printf("%s", line);
        if (fp) fprintf(fp, "%s", line);
        count++;
        p = p->next;
    }
    printf("共 %d 条记录\n", count);
    if (fp) {
        fprintf(fp, "Total: %d records\n", count);
        fclose(fp);
    }
    
    char result[32];
    sprintf(result, "Found %d records", count);
    log_query("Sales", "ALL", result);
}

void crud_user_logs(void) {
    printf("\n=== 用户全记录 ===\n");
    
    AccountNode* acards = card_get_head();
    LogInfoNode* logs = load_loginfo();
    FinanceNode* fins = finance_get_head();
    
    int cardCount = 0;
    int logCount = 0;
    int financeCount = 0;
    
    printf("------------------------------------------\n");
    printf("用户\t\t上机次数\t充值次数\t消费次数\n");
    printf("------------------------------------------\n");
    
    while (acards) {
        if (acards->data.nDel == 0) {
            const char* cardId = acards->data.aName;
            
            int loginCount = 0;
            LogInfoNode* p = logs;
            while (p) {
                if (strcmp(p->data.aCardName, cardId) == 0) {
                    loginCount++;
                }
                p = p->next;
            }
            
            int rechargeCount = 0;
            FinanceNode* f = fins;
            while (f) {
                if (strcmp(f->data.cardId, cardId) == 0 && f->data.type == 1) {
                    rechargeCount++;
                }
                f = f->next;
            }
            
            int consumeCount = 0;
            f = fins;
            while (f) {
                if (strcmp(f->data.cardId, cardId) == 0 && f->data.type == 3) {
                    consumeCount++;
                }
                f = f->next;
            }
            
            printf("%-16s\t%d\t\t%d\t\t%d\n", cardId, loginCount, rechargeCount, consumeCount);
            cardCount++;
        }
        acards = acards->next;
    }
    
    printf("------------------------------------------\n");
    printf("共计 %d 个用户\n", cardCount);
    
    char result[32];
    sprintf(result, "Found %d users", cardCount);
    log_query("UserLogs", "ALL", result);
}

void crud_user_detail(void) {
    char cardId[CARD_ID_MAX_LEN + 1];
    printf("\n=== 详细记录 ===\n");
    get_string("请输入卡号: ", cardId, sizeof(cardId));
    
    AccountNode* p = card_find(cardId);
    if (!p || p->data.nDel == 1) {
        printf("未找到该卡！\n");
        return;
    }
    
    printf("\n---------- 账户信息 ----------\n");
    printf("卡号: %s\n", p->data.aName);
    printf("状态: %s\n", p->data.nStatus[0] == '0' ? "未上机" : 
                        p->data.nStatus[0] == '1' ? "上机中" : "已注销");
    printf("余额: %.2f 元\n", p->data.fBalance);
    printf("累计消费: %.2f 元\n", p->data.fTotalUse);
    printf("使用次数: %d\n", p->data.nUseCount);
    
    if (p->data.tLast > 0) {
        struct tm* timeinfo = localtime(&p->data.tLast);
        char timeStr[32];
        strftime(timeStr, sizeof(timeStr), "%Y/%m/%d %H:%M:%S", timeinfo);
        printf("最后使用: %s\n", timeStr);
    }
    
    printf("\n---------- 上下机记录 ----------\n");
    LogInfoNode* logs = load_loginfo();
    int logCount = 0;
    float totalAmount = 0.0f;
    
    while (logs) {
        if (strcmp(logs->data.aCardName, cardId) == 0) {
            struct tm* startInfo = localtime(&logs->data.tStart);
            struct tm* endInfo = localtime(&logs->data.tEnd);
            char startTime[32], endTime[32];
            strftime(startTime, sizeof(startTime), "%Y/%m/%d %H:%M:%S", startInfo);
            strftime(endTime, sizeof(endTime), "%Y/%m/%d %H:%M:%S", endInfo);
            
            printf("%s -> %s, 消费: %.2f, 余额: %.2f\n", 
                   startTime, endTime, logs->data.fAmount, logs->data.fBalance);
            totalAmount += logs->data.fAmount;
            logCount++;
        }
        logs = logs->next;
    }
    printf("共 %d 条记录，总消费: %.2f 元\n", logCount, totalAmount);
    
    printf("\n---------- 财务记录 ----------\n");
    FinanceNode* fins = finance_get_head();
    int financeCount = 0;
    float totalRecharge = 0.0f;
    float totalRefund = 0.0f;
    
    while (fins) {
        if (strcmp(fins->data.cardId, cardId) == 0) {
            char timeStr[32] = "";
            if (strlen(fins->data.time) > 0) {
                time_t t = atol(fins->data.time);
                struct tm* timeinfo = localtime(&t);
                strftime(timeStr, sizeof(timeStr), "%Y/%m/%d %H:%M:%S", timeinfo);
            }
            printf("类型: %s, 金额: %.2f, 余额: %.2f, 时间: %s\n",
                   fins->data.type == 1 ? "充值" : (fins->data.type == 2 ? "退费" : "消费"),
                   fins->data.amount,
                   fins->data.balanceAfter,
                   timeStr);
            if (fins->data.type == 1) totalRecharge += fins->data.amount;
            else if (fins->data.type == 2) totalRefund += fins->data.amount;
            financeCount++;
        }
        fins = fins->next;
    }
    printf("共 %d 条记录，充值: %.2f 元，退费: %.2f 元\n", financeCount, totalRecharge, totalRefund);
    
    char result[64];
    sprintf(result, "Card: %s, Login: %d, Finance: %d", cardId, logCount, financeCount);
    log_query("Detail", cardId, result);
}
