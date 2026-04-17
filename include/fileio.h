#ifndef FILEIO_H
#define FILEIO_H

#include "model.h"
#include <stdio.h>

#define DATA_DIR "data/"
#define ACCOUNT_FILE DATA_DIR "accounts.bin"
#define FINANCE_FILE DATA_DIR "finance.bin"
#define LOGINOUT_FILE DATA_DIR "loginout.bin"

void ensure_data_dir(void);
int save_accounts(AccountNode* head);
AccountNode* load_accounts(void);
int save_finance(FinanceNode* head);
FinanceNode* load_finance(void);
int append_loginfo(LogInfo* info);
LogInfoNode* load_loginfo(void);

#endif
