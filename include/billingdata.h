/*
 * billingdata.h - 计费标准数据操作接口
 */

#ifndef BILLINGDATA_H
#define BILLINGDATA_H

#include "model.h"

BillingNode* billing_query(const char* packageId);
BillingNode* billing_query_all(void);
int billing_add(Billing billing);
int billing_update(Billing billing);
int billing_delete(const char* packageId);
Billing billing_get_default(void);

#endif /* BILLINGDATA_H */
