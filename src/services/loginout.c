#include "loginout.h"
#include "logindata.h"
#include "accountdata.h"
#include "utils.h"
#include "linkedlist.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

static void print_log_info(LogInfo* log) {
    char timeBuf[64];
    
    showSeparator();
    printf("卡号: %s\n", log->aCardName);
    
    formatTime(log->tStart, timeBuf, sizeof(timeBuf));
    printf("上机时间: %s\n", timeBuf);
    
    formatTime(log->tEnd, timeBuf, sizeof(timeBuf));
    printf("下机时间: %s\n", timeBuf);
    
    float duration = calculate_duration_minutes(log->tStart, log->tEnd);
    printf("时长: %.1f 分钟\n", duration);
    printf("消费: %.2f 元\n", log->fAmount);
    printf("余额: %.2f 元\n", log->fBalance);
    showSeparator();
}

static void service_user_login(void) {
    char cardId[CARD_ID_MAX_LEN + 1];
    char password[PASSWORD_MAX_LEN + 1];
    
    printf("\n用户上机\n");
    readCardId(cardId, sizeof(cardId));
    
    ListNode* result = account_query(cardId);
    if (!result) {
        printf("未找到该账户。\n");
        return;
    }
    
    Account account = *((Account*)result->data);
    list_free(result);
    
    if (account.nDel == 1) {
        printf("该账户已注销。\n");
        return;
    }
    
    printf("密码: ");
    int attempts = 0;
    
    while (attempts < MAX_LOGIN_ATTEMPTS) {
        readPassword(password, sizeof(password));
        
        if (strcmp(account.aPwd, password) == 0) {
            break;
        }
        
        attempts++;
        if (attempts < MAX_LOGIN_ATTEMPTS) {
            printf("密码错误！还剩 %d 次机会。\n", MAX_LOGIN_ATTEMPTS - attempts);
        }
    }
    
    if (attempts >= MAX_LOGIN_ATTEMPTS) {
        printf("密码错误次数过多。\n");
        return;
    }
    
    /* 执行上机 */
    if (user_login(&account, 0)) {
        account.tStart = time(NULL);
        account_update(account);
        printf("上机成功！\n");
    } else {
        printf("上机失败。\n");
    }
}

static void service_user_logout(void) {
    char cardId[CARD_ID_MAX_LEN + 1];
    
    printf("\n用户下机\n");
    readCardId(cardId, sizeof(cardId));
    
    ListNode* result = account_query(cardId);
    if (!result) {
        printf("未找到该账户。\n");
        return;
    }
    
    Account account = *((Account*)result->data);
    list_free(result);
    
    LogInfo logInfo;
    float cost = user_logout(&account, &logInfo);
    
    if (cost >= 0) {
        printf("下机成功！\n");
        printf("消费金额: %.2f 元\n", cost);
        printf("当前余额: %.2f 元\n", logInfo.fBalance);
        
        if (account.fBalance < 0) {
            printf("警告：账户已欠费 %.2f 元，请充值。\n", -account.fBalance);
        }
    } else {
        printf("下机失败。\n");
    }
}

static void service_query_login_records(void) {
    char cardId[CARD_ID_MAX_LEN + 1];
    
    printf("\n查询上下机记录\n");
    int hasCardId = readOptionalCardId(cardId, sizeof(cardId));
    
    ListNode* head;
    if (hasCardId) {
        head = login_query_by_card(cardId);
        if (!head) {
            printf("暂无该卡的记录。\n");
            return;
        }
    } else {
        head = login_query_all();
        if (!head) {
            printf("暂无记录。\n");
            return;
        }
    }
    
    ListNode* current = head;
    while (current) {
        print_log_info((LogInfo*)current->data);
        current = current->next;
    }
    list_free(head);
}

void loginout_service(void) {
    const char* options[] = {
        "用户上机",
        "用户下机",
        "查询上下机记录",
        "返回上级"
    };
    
    while (1) {
        int choice = showMenu("上下机服务", options, sizeof(options)/sizeof(options[0]));
        
        switch (choice) {
            case 0: service_user_login(); break;
            case 1: service_user_logout(); break;
            case 2: service_query_login_records(); break;
            case 3: return;
        }
    }
}
