#ifndef MODEL_H
#define MODEL_H

#include <time.h>

#define CARD_ID_MAX_LEN 18
#define PASSWORD_MAX_LEN 8
#define USERNAME_MAX_LEN 31
#define TIME_STR_MAX_LEN 31

typedef struct {
    int id;
    char username[USERNAME_MAX_LEN];
    char password[PASSWORD_MAX_LEN];
    char createTime[TIME_STR_MAX_LEN];
} Admin;

typedef struct {
    char aName[CARD_ID_MAX_LEN + 1];
    char aPwd[PASSWORD_MAX_LEN + 1];
    char nStatus[2];
    time_t tStart;
    time_t tEnd;
    float fTotalUse;
    time_t tLast;
    int nUseCount;
    float fBalance;
    int nDel;
} Account;

typedef struct AccountNode {
    Account data;
    struct AccountNode* next;
} AccountNode;

typedef struct {
    char aCardName[CARD_ID_MAX_LEN + 1];
    time_t tStart;
    time_t tEnd;
    float fAmount;
    float fBalance;
    int nPackageId;
} LogInfo;

typedef struct LogInfoNode {
    LogInfo data;
    struct LogInfoNode* next;
} LogInfoNode;

typedef struct {
    int id;
    char cardId[CARD_ID_MAX_LEN + 1];
    int type;
    float amount;
    float balanceBefore;
    float balanceAfter;
    char remark[128];
    char time[TIME_STR_MAX_LEN + 1];
} Finance;

typedef struct FinanceNode {
    Finance data;
    struct FinanceNode* next;
} FinanceNode;

#endif
