#include "menu.h"
#include "card.h"
#include "loginout.h"
#include "finance.h"
#include "crud.h"
#include "admin.h"
#include "fileio.h"
#include "utils.h"
#include "billing.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

void load_data(void) {
    card_set_head(load_accounts());
    finance_set_head(load_finance());
}

void save_data(void) {
    save_accounts(card_get_head());
    save_finance(finance_get_head());
}

void card_list_menu(void) {
    while (1) {
        printf("\n=== 卡务管理（链表） ===\n");
        printf("1. 注册新卡\n");
        printf("2. 查询卡信息\n");
        printf("3. 列出所有卡\n");
        printf("4. 注销卡\n");
        printf("0. 返回\n");
        
        int choice = get_choice(0, 4);
        if (choice == 0) break;
        
        char cardId[CARD_ID_MAX_LEN + 1];
        char password[PASSWORD_MAX_LEN + 1];
        
        switch (choice) {
            case 1:
                printf("\n注册新卡\n");
                get_string("请输入卡号: ", cardId, sizeof(cardId));
                printf("\n");
                get_string("请输入密码: ", password, sizeof(password));
                
                Account acc;
                strcpy(acc.aName, cardId);
                strcpy(acc.aPwd, password);
                strcpy(acc.nStatus, "0");
                acc.tStart = time(NULL);
                acc.tEnd = 0;
                acc.fTotalUse = 0;
                acc.tLast = 0;
                acc.nUseCount = 0;
                acc.fBalance = 0;
                acc.nDel = 0;
                
                if (card_add(&acc)) {
                    printf("注册成功！\n");
                } else {
                    printf("注册失败，卡号已存在！\n");
                }
                break;
                
            case 2:
                printf("\n查询卡信息\n");
                get_string("请输入卡号: ", cardId, sizeof(cardId));
                {
                    AccountNode* p = card_find(cardId);
                    if (p && p->data.nDel == 0) {
                        printf("\n卡号: %s\n", p->data.aName);
                        printf("状态: %s\n", p->data.nStatus[0] == '0' ? "未上机" : 
                                                    p->data.nStatus[0] == '1' ? "上机中" : "已注销");
                        printf("余额: %.2f\n", p->data.fBalance);
                        printf("累计消费: %.2f\n", p->data.fTotalUse);
                    } else {
                        printf("未找到该卡！\n");
                    }
                }
                break;
                
            case 3:
                printf("\n所有卡列表:\n");
                {
                    AccountNode* p = card_get_head();
                    int count = 0;
                    while (p) {
                        if (p->data.nDel == 0) {
                            printf("卡号: %s, 状态: %s, 余额: %.2f\n",
                                   p->data.aName,
                                   p->data.nStatus[0] == '0' ? "未上机" : 
                                       p->data.nStatus[0] == '1' ? "上机中" : "已注销",
                                   p->data.fBalance);
                            count++;
                        }
                        p = p->next;
                    }
                    printf("共 %d 张卡\n", count);
                }
                break;
                
            case 4:
                printf("\n注销卡\n");
                get_string("请输入卡号: ", cardId, sizeof(cardId));
                if (card_delete(cardId)) {
                    printf("注销成功！\n");
                } else {
                    printf("账户不存在！\n");
                }
                break;
        }
    }
}

void card_file_menu(void) {
    while (1) {
        printf("\n=== 卡务管理（文件） ===\n");
        printf("1. 查询账户\n");
        printf("2. 账户信息变更\n");
        printf("3. 注册账户\n");
        printf("4. 删除账户\n");
        printf("5. 账户列表\n");
        printf("0. 返回\n");
        
        int choice = get_choice(0, 5);
        if (choice == 0) break;
        
        char cardId[CARD_ID_MAX_LEN + 1];
        char password[PASSWORD_MAX_LEN + 1];
        
        switch (choice) {
            case 1:
                printf("\n查询账户\n");
                get_string("请输入卡号: ", cardId, sizeof(cardId));
                {
                    AccountNode* p = card_find(cardId);
                    if (p && p->data.nDel == 0) {
                        printf("\n卡号: %s\n", p->data.aName);
                        printf("状态: %s\n", p->data.nStatus[0] == '0' ? "未上机" : 
                                                    p->data.nStatus[0] == '1' ? "上机中" : "已注销");
                        printf("余额: %.2f\n", p->data.fBalance);
                        printf("累计消费: %.2f\n", p->data.fTotalUse);
                    } else {
                        int status = card_search(cardId);
                        switch (status) {
                            case 0:
                                printf("卡信息未找到\n");
                                break;
                            case 2:
                                printf("账户已删除\n");
                                break;
                        }
                    }
                    printf("查询完毕\n");
                }
                break;
                
            case 2:
                printf("\n账户信息变更\n");
                get_string("请输入卡号: ", cardId, sizeof(cardId));
                {
                    AccountNode* p = card_find(cardId);
                    if (!p) {
                        printf("账户不存在！\n");
                        break;
                    }
                    if (p->data.nDel == 1) {
                        printf("该账户已注销！\n");
                        break;
                    }
                    printf("请选择需要修改的部分：\n1.密码\n2.状态\n");
                    int cmd = get_int("输入数字标号：");
                    switch (cmd) {
                        case 1:
                            get_string("请输入新密码: ", password, sizeof(password));
                            strcpy(p->data.aPwd, password);
                            save_accounts(card_get_head());
                            printf("密码修改成功！\n");
                            break;
                        case 2:
                            {
                                int newStatus = get_int("修改卡号状态（0-未上机，1-上机中，2-已注销，3-失效）：");
                                if (newStatus >= 0 && newStatus <= 3) {
                                    p->data.nStatus[0] = '0' + newStatus;
                                    save_accounts(card_get_head());
                                    printf("状态修改成功！\n");
                                } else {
                                    printf("输入指令错误。\n");
                                }
                            }
                            break;
                        default:
                            printf("输入指令错误。\n");
                            break;
                    }
                }
                break;
                
            case 3:
                printf("\n注册账户\n");
                get_string("请输入卡号: ", cardId, sizeof(cardId));
                printf("\n");
                get_string("请输入密码: ", password, sizeof(password));
                
                {
                    float balance;
                    while (1) {
                        balance = get_float("请输入开卡金额（至少10元，最多1000元）：");
                        if (balance < 10) {
                            printf("开卡金额太少，请重新输入！\n");
                        } else if (balance > 1000) {
                            printf("开卡金额太多，请重新输入！\n");
                        } else {
                            break;
                        }
                    }
                    
                    Account acc;
                    strcpy(acc.aName, cardId);
                    strcpy(acc.aPwd, password);
                    strcpy(acc.nStatus, "0");
                    acc.tStart = time(NULL);
                    acc.tEnd = 0;
                    acc.fTotalUse = 0;
                    acc.tLast = 0;
                    acc.nUseCount = 0;
                    acc.fBalance = balance;
                    acc.nDel = 0;
                    
                    if (card_add(&acc)) {
                        save_accounts(card_get_head());
                        printf("注册成功！\n");
                    } else {
                        printf("注册失败，卡号已存在！\n");
                    }
                }
                break;
                
            case 4:
                printf("\n删除账户\n");
                get_string("请输入卡号: ", cardId, sizeof(cardId));
                {
                    AccountNode* p = card_find(cardId);
                    if (!p) {
                        printf("账户不存在！\n");
                        break;
                    }
                    if (p->data.nDel == 1) {
                        printf("该账户已注销！\n");
                        break;
                    }
                    
                    printf("账户信息：\n");
                    printf("  卡号：%s\n", p->data.aName);
                    printf("  余额：%.2f 元\n", p->data.fBalance);
                    
                    if (p->data.fBalance > 0) {
                        printf("提示：该账户还有余额，请先进行退费操作后再注销！\n");
                        printf("确定要注销该账户吗？(y/n)：");
                        char confirm[10];
                        scanf("%s", confirm);
                        getchar();
                        if (confirm[0] != 'y' && confirm[0] != 'Y') {
                            printf("已取消注销\n");
                            break;
                        }
                        p->data.fBalance = 0;
                    } else if (p->data.fBalance < 0) {
                        printf("提示：该账户欠费 %.2f 元，请先还清欠款后再注销！\n", p->data.fBalance);
                        printf("已取消注销\n");
                        break;
                    }
                    
                    p->data.nDel = 1;
                    p->data.nStatus[0] = '2';
                    p->data.tEnd = time(NULL);
                    save_accounts(card_get_head());
                    printf("注销成功！\n");
                }
                break;
                
            case 5:
                printf("\n账户列表:\n");
                printf("卡号\t\t状态\t余额\t使用次数\t最后使用时间\n");
                printf("------------------------------------------------------------\n");
                {
                    AccountNode* p = card_get_head();
                    int count = 0;
                    float totalBalance = 0;
                    while (p) {
                        if (p->data.nDel == 0) {
                            char statusStr[20] = "未上机";
                            if (p->data.nStatus[0] == '1') strcpy(statusStr, "上机中");
                            else if (p->data.nStatus[0] == '2') strcpy(statusStr, "已注销");
                            else if (p->data.nStatus[0] == '3') strcpy(statusStr, "失效");
                            
                            char timeStr[32] = "未使用";
                            if (p->data.tLast > 0) {
                                struct tm* timeinfo = localtime(&p->data.tLast);
                                if (timeinfo) strftime(timeStr, sizeof(timeStr), "%Y/%m/%d %H:%M", timeinfo);
                            }
                            
                            printf("%s\t%s\t%.2f\t%d\t\t%s\n",
                                   p->data.aName,
                                   statusStr,
                                   p->data.fBalance,
                                   p->data.nUseCount,
                                   timeStr);
                            totalBalance += p->data.fBalance;
                            count++;
                        }
                        p = p->next;
                    }
                    printf("------------------------------------------------------------\n");
                    printf("共计 %d 个有效账户，总余额：%.2f 元\n", count, totalBalance);
                }
                break;
        }
    }
}

void loginout_menu(void) {
    while (1) {
        printf("\n=== 上下机服务 ===\n");
        printf("1. 上机\n");
        printf("2. 下机\n");
        printf("0. 返回\n");
        
        int choice = get_choice(0, 2);
        if (choice == 0) break;
        
        char cardId[CARD_ID_MAX_LEN + 1];
        char password[PASSWORD_MAX_LEN + 1];
        
        switch (choice) {
            case 1:
                printf("\n上机\n");
                get_string("请输入卡号: ", cardId, sizeof(cardId));
                get_string("请输入密码: ", password, sizeof(password));
                login_on(cardId, password);
                break;
                
            case 2:
                printf("\n下机\n");
                get_string("请输入卡号: ", cardId, sizeof(cardId));
                login_off(cardId);
                break;
        }
    }
}

void finance_menu(void) {
    while (1) {
        printf("\n=== 充值退费 ===\n");
        printf("1. 充值\n");
        printf("2. 退费\n");
        printf("0. 返回\n");
        
        int choice = get_choice(0, 2);
        if (choice == 0) break;
        
        char cardId[CARD_ID_MAX_LEN + 1];
        
        switch (choice) {
            case 1:
                printf("\n充值\n");
                get_string("请输入卡号: ", cardId, sizeof(cardId));
                {
                    AccountNode* p = card_find(cardId);
                    if (p) {
                        float amount = get_float("请输入充值金额: ");
                        recharge(cardId, amount);
                    } else {
                        printf("账户不存在！\n");
                    }
                }
                break;
                
            case 2:
                printf("\n退费\n");
                get_string("请输入卡号: ", cardId, sizeof(cardId));
                {
                    AccountNode* p = card_find(cardId);
                    if (p) {
                        float amount = get_float("请输入退费金额: ");
                        refund(cardId, amount);
                    } else {
                        printf("账户不存在！\n");
                    }
                }
                break;
        }
    }
}

void main_menu(void) {
    load_data();
    
    while (1) {
        printf("\n========== 计费管理系统 ==========\n");
        printf("1. 卡务管理（链表）\n");
        printf("2. 卡务管理（文件）\n");
        printf("3. 上下机服务\n");
        printf("4. 充值退费\n");
        printf("5. 查询业务\n");
        printf("6. 计费标准设置\n");
        printf("7. 管理员管理\n");
        printf("0. 退出系统\n");
        
        int choice = get_choice(0, 7);
        if (choice == 0) {
            save_data();
            admin_save();
            admin_logout();
            printf("正在退出系统\n");
            break;
        }
        
        switch (choice) {
            case 1:
                card_list_menu();
                break;
            case 2:
                card_file_menu();
                break;
            case 3:
                loginout_menu();
                break;
            case 4:
                finance_menu();
                break;
            case 5:
                crud_menu();
                break;
            case 6:
                billing_menu();
                break;
            case 7:
                admin_menu();
                break;
        }
    }
}
