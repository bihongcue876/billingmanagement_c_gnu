/*
 * model.h - 核心数据结构定义
 */

#ifndef MODEL_H
#define MODEL_H

#include <time.h>

/* 计费单位枚举 */
typedef enum {
    UNIT_MINUTE,  /* 分钟计费 */
    UNIT_HOUR     /* 小时计费 */
} UnitType;

/* 字符串最大长度常量 */
#define CARD_ID_MAX_LEN      18   /* 卡号最大长度 */
#define PASSWORD_MAX_LEN     8    /* 密码最大长度 */
#define STATUS_MAX_LEN       1    /* 状态最大长度 */
#define USERNAME_MAX_LEN     31   /* 用户名最大长度 */
#define PACKAGE_ID_MAX_LEN   31   /* 套餐编号最大长度 */
#define REMARK_MAX_LEN       127  /* 备注最大长度 */
#define TIME_STR_MAX_LEN     31   /* 时间字符串最大长度 */

/* 业务常量 */
#define MAX_LOGIN_ATTEMPTS 5
#define MAX_INITIAL_BALANCE 1000.0f
#define MAX_RECHARGE_AMOUNT 10000.0f
#define LARGE_AMOUNT_THRESHOLD 500.0f
#define MAX_CARD_ID_INPUT_LEN 18

/* 计费规则常量 */
#define BILLING_TIER_SHORT_MIN 15.0f
#define BILLING_TIER_LONG_MIN 60.0f

/* 管理员信息结构 */
typedef struct {
    int id;
    char username[USERNAME_MAX_LEN + 1];
    char password[PASSWORD_MAX_LEN + 24];
    char createTime[TIME_STR_MAX_LEN + 1];  /* 格式: YYYY-MM-DD HH:MM:SS */
} Admin;

/* 用户账户信息 */
typedef struct {
    char aName[CARD_ID_MAX_LEN + 1];     /* 卡号 */
    char aPwd[PASSWORD_MAX_LEN + 1];     /* 密码 */
    char nStatus[STATUS_MAX_LEN + 1];    /* 状态: "0"-未上机, "1"-上机中, "2"-已注销 */
    time_t tStart;                       /* 开卡时间 */
    time_t tEnd;                         /* 截止时间 */
    float fTotalUse;                     /* 累计消费 */
    time_t tLast;                        /* 最后使用时间 */
    int nUseCount;                       /* 使用次数 */
    float fBalance;                      /* 余额 */
    int nDel;                            /* 删除标记: 0-有效, 1-已删除 */
} Account;

/* 计费套餐 */
typedef struct {
    char sPackageId[PACKAGE_ID_MAX_LEN + 1]; /* 套餐编号 */
    UnitType nUnitType;                      /* 计费单位 */
    float fUnitPrice;                        /* 单价 */
    int nDel;                                /* 失效标记: 0-有效, 1-已删除 */
} Billing;

/* 上下机记录 */
typedef struct {
    char aCardName[CARD_ID_MAX_LEN + 1]; /* 卡号 */
    time_t tStart;                       /* 上机时间 */
    time_t tEnd;                         /* 下机时间 */
    float fAmount;                       /* 消费金额 */
    float fBalance;                      /* 消费后余额 */
    int nPackageId;                      /* 套餐ID */
} LogInfo;

/* 财务交易记录 */
typedef struct {
    int id;                              /* 交易ID */
    char cardId[CARD_ID_MAX_LEN + 1];    /* 卡号 */
    int type;                            /* 类型: 1-充值, 2-退费, 3-消费 */
    float amount;                        /* 金额 */
    float balanceBefore;                 /* 交易前余额 */
    float balanceAfter;                  /* 交易后余额 */
    char remark[REMARK_MAX_LEN + 1];     /* 备注 */
    char time[TIME_STR_MAX_LEN + 1];     /* 交易时间 */
} Finance;

/* 通用链表节点类型别名 */
typedef struct ListNode AccountNode;
typedef struct ListNode BillingNode;
typedef struct ListNode LogInfoNode;
typedef struct ListNode FinanceNode;
typedef struct ListNode AdminNode;

#endif /* MODEL_H */
