/*
 * accountdata.h - 账户数据操作接口
 */

#ifndef ACCOUNTDATA_H
#define ACCOUNTDATA_H

#include "model.h"

AccountNode* account_query(const char* cardId);
AccountNode* account_query_all(void);
int account_register(Account account);
int account_deactivate(const char* cardId);
int account_reactivate(const char* cardId);
int account_update(Account account);
float account_get_total_balance(void);

#endif /* ACCOUNTDATA_H */
