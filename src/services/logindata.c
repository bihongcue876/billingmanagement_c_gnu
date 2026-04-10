#include "logindata.h"
#include "billingdata.h"
#include "accountdata.h"
#include "fileio.h"
#include "linkedlist.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>

float calculate_duration_minutes(time_t start, time_t end) {
    double diff = difftime(end, start);
    return (float)(diff / 60.0);
}

float calculate_cost(float durationMinutes, Billing billing) {
    if (billing.nUnitType == UNIT_MINUTE) {
        /* 分钟计费规则 */
        if (durationMinutes < BILLING_TIER_SHORT_MIN) {
            /* <15分钟向下取整 */
            return floorf(durationMinutes) * billing.fUnitPrice;
        } else if (durationMinutes < BILLING_TIER_LONG_MIN) {
            /* 15-60分钟向上取整 */
            return ceilf(durationMinutes) * billing.fUnitPrice;
        } else {
            /* >=60分钟向下取整 */
            return floorf(durationMinutes) * billing.fUnitPrice;
        }
    } else {
        /* 小时计费：全部向上取整 */
        float hours = durationMinutes / 60.0f;
        return ceilf(hours) * billing.fUnitPrice;
    }
}

int user_login(Account* account, int packageId) {
    (void)packageId;  /* 当前未使用套餐参数 */
    
    if (account->nDel == 1) {
        printf("该账户已注销，无法上机。\n");
        return 0;
    }
    
    if (strcmp(account->nStatus, "1") == 0) {
        printf("该账户已在上机中。\n");
        return 0;
    }
    
    if (account->fBalance <= 0) {
        printf("余额不足，请充值。\n");
        return 0;
    }
    
    /* 更新状态为上机中 */
    strncpy(account->nStatus, "1", sizeof(account->nStatus) - 1);
    account->nStatus[sizeof(account->nStatus) - 1] = '\0';
    
    return account_update(*account);
}

float user_logout(Account* account, LogInfo* logInfo) {
    if (strcmp(account->nStatus, "1") != 0) {
        printf("该账户未在上机状态。\n");
        return -1.0f;
    }
    
    /* 获取套餐信息 */
    BillingNode* billingResult = billing_query_all();
    if (!billingResult) {
        printf("无可用计费套餐。\n");
        return -1.0f;
    }
    
    /* 使用第一个可用套餐 */
    Billing billing = *((Billing*)billingResult->data);
    list_free(billingResult);
    
    /* 计算时长和费用 */
    time_t endTime = time(NULL);
    float duration = calculate_duration_minutes(account->tStart, endTime);
    float cost = calculate_cost(duration, billing);
    
    /* 更新账户信息 */
    account->fTotalUse += cost;
    account->fBalance -= cost;
    account->tLast = endTime;
    account->nUseCount++;
    strncpy(account->nStatus, "0", sizeof(account->nStatus) - 1);
    account->nStatus[sizeof(account->nStatus) - 1] = '\0';
    
    /* 保存账户 */
    account_update(*account);
    
    /* 创建日志记录 */
    strncpy(logInfo->aCardName, account->aName, sizeof(logInfo->aCardName) - 1);
    logInfo->aCardName[sizeof(logInfo->aCardName) - 1] = '\0';
    logInfo->tStart = account->tStart;
    logInfo->tEnd = endTime;
    logInfo->fAmount = cost;
    logInfo->fBalance = account->fBalance;
    logInfo->nPackageId = 0;
    
    /* 保存到文件 */
    time_t now = time(NULL);
    struct tm* tmInfo = localtime(&now);
    int year = tmInfo->tm_year + 1900;
    
    char filename[128];
    get_loginout_filename(year, filename, sizeof(filename));
    file_append_record(filename, FILE_MAGIC_LOGINFO, logInfo, sizeof(LogInfo));
    
    return cost;
}

LogInfoNode* login_query_by_card(const char* cardId) {
    /* 读取当前年份的文件 */
    time_t now = time(NULL);
    struct tm* tmInfo = localtime(&now);
    int year = tmInfo->tm_year + 1900;
    
    char filename[128];
    get_loginout_filename(year, filename, sizeof(filename));
    
    ListNode* head = file_read_all(filename, FILE_MAGIC_LOGINFO, sizeof(LogInfo));
    if (!head) return NULL;
    
    ListNode* result = NULL;
    ListNode* current = head;
    
    while (current) {
        LogInfo* log = (LogInfo*)current->data;
        if (strcmp(log->aCardName, cardId) == 0) {
            result = list_append(result, current->data, sizeof(LogInfo));
        }
        current = current->next;
    }
    
    list_free(head);
    return result;
}

LogInfoNode* login_query_all(void) {
    time_t now = time(NULL);
    struct tm* tmInfo = localtime(&now);
    int year = tmInfo->tm_year + 1900;
    
    char filename[128];
    get_loginout_filename(year, filename, sizeof(filename));
    
    return file_read_all(filename, FILE_MAGIC_LOGINFO, sizeof(LogInfo));
}

LogInfoNode* login_query_by_date(int year, int month, int day) {
    char filename[128];
    get_loginout_filename(year, filename, sizeof(filename));
    
    ListNode* head = file_read_all(filename, FILE_MAGIC_LOGINFO, sizeof(LogInfo));
    if (!head) return NULL;
    
    time_t startTime, endTime;
    build_date_range(year, month, day, &startTime, &endTime);
    
    /* 过滤该日期的记录 */
    ListNode* result = NULL;
    ListNode* current = head;
    
    while (current) {
        LogInfo* log = (LogInfo*)current->data;
        if (log->tStart >= startTime && log->tStart < endTime) {
            result = list_append(result, current->data, sizeof(LogInfo));
        }
        current = current->next;
    }
    
    list_free(head);
    return result;
}
