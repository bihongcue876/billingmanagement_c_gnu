#ifndef FINANCE_H
#define FINANCE_H

#include "model.h"

void finance_set_head(FinanceNode* h);
FinanceNode* finance_get_head(void);
int recharge(const char* cardId, float amount);
int refund(const char* cardId, float amount);

#endif
