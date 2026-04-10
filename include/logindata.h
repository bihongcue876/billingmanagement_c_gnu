/*
 * logindata.h - 上下机数据操作接口
 */

#ifndef LOGINDATA_H
#define LOGINDATA_H

#include "model.h"

int user_login(Account* account, int packageId);
float user_logout(Account* account, LogInfo* logInfo);
LogInfoNode* login_query_by_card(const char* cardId);
LogInfoNode* login_query_all(void);
LogInfoNode* login_query_by_date(int year, int month, int day);
float calculate_duration_minutes(time_t start, time_t end);
float calculate_cost(float durationMinutes, Billing billing);

#endif /* LOGINDATA_H */
