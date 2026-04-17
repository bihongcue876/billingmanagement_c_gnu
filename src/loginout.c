#include "loginout.h"
#include "card.h"
#include "fileio.h"
#include "billing.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

float calculate_amount(double minutes) {
    double hours = minutes / 60.0;
    return (float)calculate_fee(hours);
}

int login_on(const char* cardId, const char* password) {
    AccountNode* p = card_find(cardId);
    if (!p) {
        printf("卡号不存在！\n");
        return 0;
    }
    
    if (p->data.nDel != 0) {
        printf("该卡已注销！\n");
        return 0;
    }
    
    if (strcmp(p->data.aPwd, password) != 0) {
        printf("密码错误！\n");
        return 0;
    }
    
    if (p->data.nStatus[0] == '1') {
        printf("该卡已在上机！\n");
        return 0;
    }
    
    if (p->data.fBalance <= 0) {
        printf("余额不足，请先充值！\n");
        return 0;
    }
    
    p->data.nStatus[0] = '1';
    p->data.tStart = time(NULL);
    printf("上机成功！\n");
    return 1;
}

int login_off(const char* cardId) {
    AccountNode* p = card_find(cardId);
    if (!p) {
        printf("卡号不存在！\n");
        return 0;
    }
    
    if (p->data.nStatus[0] != '1') {
        printf("该卡未上机！\n");
        return 0;
    }
    
    time_t now = time(NULL);
    double minutes = difftime(now, p->data.tStart) / 60.0;
    
    float amount = calculate_amount(minutes);
    
    if (amount > p->data.fBalance) {
        amount = p->data.fBalance;
    }
    
    p->data.fBalance -= amount;
    p->data.fTotalUse += amount;
    p->data.nUseCount++;
    p->data.nStatus[0] = '0';
    p->data.tEnd = now;
    p->data.tLast = now;
    
    LogInfo log;
    strcpy(log.aCardName, cardId);
    log.tStart = p->data.tStart;
    log.tEnd = now;
    log.fAmount = amount;
    log.fBalance = p->data.fBalance;
    append_loginfo(&log);
    
    printf("下机成功！消费 %.2f 元（%.0f分钟），余额 %.2f 元\n", amount, minutes, p->data.fBalance);
    return 1;
}
