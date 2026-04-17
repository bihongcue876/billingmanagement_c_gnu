#include "menu.h"
#include "card.h"
#include "loginout.h"
#include "finance.h"
#include "crud.h"
#include "admin.h"
#include "fileio.h"
#include "utils.h"
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
                    printf("注销失败，卡号不存在！\n");
                }
                break;
        }
    }
}

void card_file_menu(void) {
    while (1) {
        printf("\n=== 卡务管理（文件） ===\n");
        printf("1. 从文件加载卡\n");
        printf("2. 保存卡到文件\n");
        printf("3. 充值\n");
        printf("4. 退费\n");
        printf("0. 返回\n");
        
        int choice = get_choice(0, 4);
        if (choice == 0) break;
        
        char cardId[CARD_ID_MAX_LEN + 1];
        
        switch (choice) {
            case 1:
                printf("\n从文件加载...\n");
                card_set_head(load_accounts());
                printf("加载成功！\n");
                break;
                
            case 2:
                printf("\n保存到文件...\n");
                save_accounts(card_get_head());
                printf("保存成功！\n");
                break;
                
            case 3:
                printf("\n充值\n");
                get_string("请输入卡号: ", cardId, sizeof(cardId));
                {
                    AccountNode* p = card_find(cardId);
                    if (p) {
                        float amount = get_float("请输入充值金额: ");
                        recharge(cardId, amount);
                    } else {
                        printf("卡号不存在！\n");
                    }
                }
                break;
                
            case 4:
                printf("\n退费\n");
                get_string("请输入卡号: ", cardId, sizeof(cardId));
                {
                    AccountNode* p = card_find(cardId);
                    if (p) {
                        float amount = get_float("请输入退费金额: ");
                        refund(cardId, amount);
                    } else {
                        printf("卡号不存在！\n");
                    }
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
                        printf("卡号不存在！\n");
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
                        printf("卡号不存在！\n");
                    }
                }
                break;
        }
    }
}

void main_menu(void) {
    if (!admin_login()) {
        return;
    }
    
    load_data();
    
    while (1) {
        printf("\n========== 计费管理系统 ==========\n");
        printf("1. 卡务管理（链表）\n");
        printf("2. 卡务管理（文件）\n");
        printf("3. 上下机服务\n");
        printf("4. 充值退费\n");
        printf("5. 查询业务\n");
        printf("0. 退出系统\n");
        
        int choice = get_choice(0, 5);
        if (choice == 0) {
            save_data();
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
        }
    }
}
