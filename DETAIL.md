# 计费管理系统 - 详细设计文档

## 一、项目概述

本项目是对原C++_CLI计费管理系统（基于SQLite）的C语言重构版本。核心目标：
- 移除所有数据库依赖
- 使用纯C99标准实现
- 采用二进制文件格式存储数据
- 保持与原系统完全一致的功能

## 二、架构设计

### 2.1 三层架构

```
表示层 (UI)          →  各*service.c文件 (菜单交互)
        ↓
业务服务层           →  各*data.c文件 (业务逻辑)
        ↓
数据访问层           →  fileio.c (二进制文件读写)
```

### 2.2 目录结构

```
billingmanagement_C_GNU/
├── include/
│   ├── model.h           # 核心数据结构定义
│   ├── linkedlist.h      # 链表操作接口
│   ├── fileio.h          # 文件IO接口
│   ├── utils.h           # 通用工具函数
│   ├── admin.h           # 管理员登录接口
│   ├── accountdata.h     # 账户数据接口
│   ├── billingdata.h     # 计费标准数据接口
│   ├── logindata.h       # 上下机数据接口
│   ├── financedata.h     # 财务数据接口
│   ├── *service.h        # 各服务菜单接口
│   ├── menu.h            # 主菜单接口
│   └── service.h         # 服务调度接口
│
├── src/
│   ├── Main.c            # 程序入口
│   ├── linkedlist.c      # 链表实现
│   ├── fileio.c          # 二进制文件读写实现
│   ├── utils.c           # 工具函数实现
│   ├── admin.c           # 管理员登录实现
│   ├── menu.c            # 主菜单实现
│   ├── service.c         # 服务调度实现
│   ├── adminservice.c    # 管理员管理实现
│   ├── accountdata.c     # 账户数据操作实现
│   └── services/
│       ├── accountservice.c    # 账户服务
│       ├── billingdatabase.c   # 计费标准数据操作
│       ├── billingstandard.c   # 计费标准服务
│       ├── logindata.c         # 上下机数据操作
│       ├── loginout.c          # 上下机服务
│       ├── financedata.c       # 财务数据操作
│       ├── financeservice.c    # 财务服务
│       └── logsearch.c         # 查询统计服务
│
└── data/                 # 运行时自动创建
    ├── admins.bin        # 管理员数据
    ├── accounts.bin      # 账户数据
    ├── billings.bin      # 计费套餐数据
    ├── finance.bin       # 财务交易记录
    └── loginout_YYYY.bin # 上下机日志 (按年分文件)
```

## 三、核心数据结构

### 3.1 账户 (Account)
```c
typedef struct {
    char aName[19];      // 卡号 (max 18 chars + '\0')
    char aPwd[9];        // 密码 (max 8 chars + '\0')
    char nStatus[2];     // 状态: "0"-未上机, "1"-上机中, "2"-已注销, "3"-失效
    time_t tStart;       // 开卡时间 (Unix时间戳)
    time_t tEnd;         // 截止时间
    float fTotalUse;     // 累计消费
    time_t tLast;        // 最后使用时间
    int nUseCount;       // 使用次数
    float fBalance;      // 余额
    int nDel;            // 删除标记: 0-有效, 1-已删除
} Account;
```

### 3.2 计费套餐 (Billing)
```c
typedef struct {
    char sPackageId[32]; // 套餐编号
    UnitType nUnitType;  // 计费单位: UNIT_MINUTE(0) 或 UNIT_HOUR(1)
    float fUnitPrice;    // 单价
    int nDel;            // 失效标记: 0-有效, 1-已删除
} Billing;
```

### 3.3 上下机记录 (LogInfo)
```c
typedef struct {
    char aCardName[19];  // 卡号
    time_t tStart;       // 上机时间
    time_t tEnd;         // 下机时间
    float fAmount;       // 消费金额
    float fBalance;      // 消费后余额
    int nPackageId;      // 套餐ID
} LogInfo;
```

### 3.4 财务记录 (Finance)
```c
typedef struct {
    int id;              // 交易ID (自增)
    char cardId[19];     // 卡号
    int type;            // 类型: 1-充值, 2-退费, 3-消费
    float amount;        // 金额
    float balanceBefore; // 交易前余额
    float balanceAfter;  // 交易后余额
    char remark[128];    // 备注
    char time[32];       // 交易时间 "YYYY-MM-DD HH:MM:SS"
} Finance;
```

### 3.5 管理员 (Admin)
```c
typedef struct {
    int id;
    char username[32];
    char password[32];
    char createTime[32];  // "YYYY-MM-DD HH:MM:SS"
} Admin;
```

### 3.6 链表节点

每个数据结构都有对应的链表节点类型：
```c
typedef struct AccountNode {
    Account data;
    struct AccountNode* next;
} AccountNode;

// 类似地: BillingNode, LogInfoNode, FinanceNode, AdminNode
```

## 四、文件存储格式

### 4.1 二进制文件结构

所有数据文件采用统一的二进制格式：

```
┌─────────────────────────────────┐
│  FileHeader (12字节)            │
│  ├─ magic (4字节, unsigned int) │ 魔数标识
│  ├─ version (4字节, unsigned int)│ 版本号 (当前为1)
│  └─ count (4字节, unsigned int) │ 记录数量
├─────────────────────────────────┤
│  Record 1 (结构体大小)          │
├─────────────────────────────────┤
│  Record 2                       │
├─────────────────────────────────┤
│  ...                            │
└─────────────────────────────────┘
```

### 4.2 魔数标识

| 文件 | 魔数 (十六进制) | 含义 |
|------|----------------|------|
| admins.bin | `0x41444D4E` | "ADMN" |
| accounts.bin | `0x41434354` | "ACCT" |
| billings.bin | `0x424C4C4E` | "BLLN" |
| finance.bin | `0x46494E4E` | "FINN" |
| loginout_YYYY.bin | `0x4C4F4749` | "LOGI" |

### 4.3 文件路径

```c
#define DATA_DIR "data/"
#define ADMIN_FILE DATA_DIR "admins.bin"
#define ACCOUNT_FILE DATA_DIR "accounts.bin"
#define BILLING_FILE DATA_DIR "billings.bin"
#define FINANCE_FILE DATA_DIR "finance.bin"
#define LOGINOUT_FILE_PREFIX DATA_DIR "loginout_"
#define LOGINOUT_FILE_SUFFIX ".bin"
```

## 五、核心模块详解

### 5.1 链表模块 (linkedlist.c)

提供完整的CRUD操作：

```c
// 创建节点
AccountNode* account_node_create(Account data);

// 释放链表
void account_list_free(AccountNode* head);

// 获取链表大小
int account_list_size(AccountNode* head);

// 追加节点
AccountNode* account_list_append(AccountNode* head, Account data);

// 删除节点
AccountNode* account_list_remove(AccountNode* head, const char* cardId);

// 查找节点
AccountNode* account_list_find(AccountNode* head, const char* cardId);
```

### 5.2 文件IO模块 (fileio.c)

提供三种操作模式：

```c
// 读取全部记录到链表
AccountNode* account_file_read_all(const char* filename);

// 写回全部记录 (覆盖写入)
int account_file_write_all(const char* filename, AccountNode* head);

// 追加单条记录 (读-改-写)
int account_file_append(const char* filename, Account account);

// 更新单条记录 (读-改-写)
int account_file_update(const char* filename, Account account);
```

**优化策略**：
- 小数据量：全量读写，简单可靠
- 追加模式：尝试直接追加到文件末尾（更新header.count）
- 文件不存在时自动创建data目录

### 5.3 计费规则 (logindata.c)

#### 分钟计费规则
```c
if (durationMinutes < 15.0f) {
    // <15分钟：向下取整
    return floorf(durationMinutes) * billing.fUnitPrice;
} else if (durationMinutes < 60.0f) {
    // 15-60分钟：向上取整
    return ceilf(durationMinutes) * billing.fUnitPrice;
} else {
    // >=60分钟：向下取整
    return floorf(durationMinutes) * billing.fUnitPrice;
}
```

#### 小时计费规则
```c
// 全部向上取整
float hours = durationMinutes / 60.0f;
return ceilf(hours) * billing.fUnitPrice;
```

### 5.4 账户状态机

```
注册 → "0" (未上机)
   ↓
上机 → "1" (上机中)
   ↓
下机 → "0" (未上机)

注销 → "2" (已注销) → 可重新启用 → "0"
```

### 5.5 财务交易类型

```c
enum {
    FINANCE_RECHARGE = 1,  // 充值
    FINANCE_REFUND = 2,    // 退费
    FINANCE_CONSUME = 3    // 消费 (下机时自动生成)
};
```

## 六、工具函数

### 6.1 输入处理 (utils.c)

```c
// 带范围检查的浮点数读取
float readFloat(const char* prompt, float minVal, float maxVal);

// 密码隐藏输入 (使用_getch显示*号)
void readPassword(char* password, int maxSize);

// 卡号验证 (字母/数字/下划线, max 18位)
void readCardId(char* cardId, int maxSize);

// 菜单显示
int showMenu(const char* title, const char** options, int optionCount);
```

### 6.2 时间处理

```c
// 时间戳 → 字符串
void formatTime(time_t t, char* buffer, int bufferSize);
// 输出: "YYYY-MM-DD HH:MM:SS"

// 字符串 → 时间戳
time_t parseTime(const char* timeStr);

// 获取当前时间
void getCurrentTime(char* buffer, int bufferSize);
```

## 七、编译配置

### compile.bat
```batch
@echo off
gcc -std=c99 -Wall -Wextra -Iinclude ^
    src\Main.c ^
    src\utils.c ^
    src\linkedlist.c ^
    src\fileio.c ^
    src\admin.c ^
    src\menu.c ^
    src\service.c ^
    src\adminservice.c ^
    src\accountdata.c ^
    src\services\accountservice.c ^
    src\services\billingdatabase.c ^
    src\services\billingstandard.c ^
    src\services\logindata.c ^
    src\services\loginout.c ^
    src\services\financedata.c ^
    src\services\financeservice.c ^
    src\services\logsearch.c ^
    -o app.exe -lws2_32
```

**编译参数说明**：
- `-std=c99`: 使用C99标准
- `-Wall -Wextra`: 开启所有警告
- `-Iinclude`: 头文件搜索路径
- `-lws2_32`: Windows Socket库 (预留扩展)

## 八、安全特性

### 8.1 密码保护
- 所有密码输入使用`_getch()`隐藏显示
- 登录失败限制 (管理员5次失败锁定)

### 8.2 软删除机制
- 账户/套餐/管理员均采用标记删除 (nDel字段)
- 保留数据可追溯，支持恢复操作

### 8.3 输入验证
- 卡号格式检查 (字母/数字/下划线, 1-18位)
- 金额范围检查 (0-1000元注册限制)
- 大额操作二次确认 (>500元充值/退费)

### 8.4 数据完整性
- 文件魔数标识验证
- 版本号控制
- 记录数量校验

## 九、与C++版本对照表

| 特性 | C++版本 | C版本 |
|------|---------|-------|
| 存储 | SQLite数据库 (.db) | 二进制文件 (.bin) |
| 动态数组 | `std::vector<T>` | 手动链表实现 |
| 字符串 | `std::string` | `char[]` 固定数组 |
| 查询 | SQL SELECT | 链表遍历 |
| 插入 | SQL INSERT | 链表append + 文件写回 |
| 更新 | SQL UPDATE | 链表find + 文件写回 |
| 删除 | SQL DELETE | 软删除标记 |
| 事务 | SQLite事务 | 无 (单条操作) |
| 预编译语句 | `sqlite3_prepare_v2` | 不适用 |
| 编译 | CMake + g++ | compile.bat + gcc |

## 十、运行流程

```
main()
  ├─ setlocale(LC_ALL, "chs")  // 设置中文本地化
  ├─ admin_init_defaults()     // 初始化默认管理员
  └─ main_menu()               // 进入主菜单循环
       ├─ showMenu()           // 显示7个选项
       └─ service_dispatch()   // 根据选择调用对应服务
            ├─ 1 → admin_service()
            ├─ 2 → account_service()
            ├─ 3 → loginout_service()
            ├─ 4 → billing_standard_service()
            ├─ 5 → finance_service()
            └─ 6 → log_search_service()
```

## 十一、数据文件示例

### 账户文件结构 (accounts.bin)
```
偏移   内容
0x00   FileHeader: magic=0x41434354, version=1, count=N
0x0C   Account[0]: aName[19], aPwd[9], nStatus[2], padding, ...
0xXX   Account[1]: ...
...
```

### 财务记录查询示例
```
交易ID: 1
卡号: user001
类型: 充值
金额: 100.00 元
交易前余额: 0.00 元
交易后余额: 100.00 元
备注: 账户充值
时间: 2026-04-10 14:30:00
```

## 十二、扩展建议

1. **并发控制**：可添加文件锁机制 (flock) 支持多进程访问
2. **索引优化**：大数据量时可考虑添加哈希索引或B树
3. **日志备份**：上下机日志按年分文件，可添加按月分文件
4. **数据导出**：支持导出为CSV/TXT格式
5. **配置管理**：添加配置文件支持自定义默认参数
