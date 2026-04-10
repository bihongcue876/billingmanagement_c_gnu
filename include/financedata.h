/*
 * financedata.h - 财务数据操作接口
 */

#ifndef FINANCEDATA_H
#define FINANCEDATA_H

#include "model.h"

FinanceNode* finance_query_by_card(const char* cardId);
FinanceNode* finance_query_all(void);
FinanceNode* finance_query_by_date(int year, int month, int day);
int finance_add(Finance finance);
float finance_statistics(int type);
int finance_get_next_id(void);

#endif /* FINANCEDATA_H */
