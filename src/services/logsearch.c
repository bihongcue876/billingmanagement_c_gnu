#include "logsearch.h"
#include "financedata.h"
#include "logindata.h"
#include "utils.h"
#include "linkedlist.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

static void print_finance_info(Finance* f) {
    showSeparator();
    printf("交易ID: %d\n", f->id);
    printf("卡号: %s\n", f->cardId);
    printf("类型: ");
    switch (f->type) {
        case 1: printf("充值\n"); break;
        case 2: printf("退费\n"); break;
        case 3: printf("消费\n"); break;
        default: printf("未知\n"); break;
    }
    printf("金额: %.2f 元\n", f->amount);
    printf("备注: %s\n", f->remark);
    printf("时间: %s\n", f->time);
    showSeparator();
}

static void print_log_info(LogInfo* log) {
    char timeBuf[64];
    
    showSeparator();
    printf("卡号: %s\n", log->aCardName);
    
    formatTime(log->tStart, timeBuf, sizeof(timeBuf));
    printf("上机时间: %s\n", timeBuf);
    
    formatTime(log->tEnd, timeBuf, sizeof(timeBuf));
    printf("下机时间: %s\n", timeBuf);
    
    float duration = calculate_duration_minutes(log->tStart, log->tEnd);
    printf("时长: %.1f 分钟\n", duration);
    printf("消费: %.2f 元\n", log->fAmount);
    printf("余额: %.2f 元\n", log->fBalance);
    showSeparator();
}

static void service_revenue_log(void) {
    printf("\n-----------------------------------\n");
    printf("营业额日志\n");
    printf("-----------------------------------\n");
    
    ListNode* head = finance_query_all();
    if (!head) {
        printf("暂无交易记录。\n");
        return;
    }
    
    float recharge = 0.0f, refund = 0.0f, consume = 0.0f;
    ListNode* current = head;
    
    while (current) {
        Finance* f = (Finance*)current->data;
        print_finance_info(f);
        
        switch (f->type) {
            case 1: recharge += f->amount; break;
            case 2: refund += f->amount; break;
            case 3: consume += f->amount; break;
        }
        
        current = current->next;
    }
    
    printf("\n-----------------------------------\n");
    printf("统计汇总\n");
    printf("-----------------------------------\n");
    printf("充值总额: %.2f 元\n", recharge);
    printf("退费总额: %.2f 元\n", refund);
    printf("消费总额: %.2f 元\n", consume);
    printf("净营业额: %.2f 元\n", recharge - refund);
    showSeparator();
    
    list_free(head);
}

static void service_loginout_log(void) {
    printf("\n-----------------------------------\n");
    printf("上下机日志\n");
    printf("-----------------------------------\n");
    
    ListNode* head = login_query_all();
    if (!head) {
        printf("暂无上下机记录。\n");
        return;
    }
    
    ListNode* current = head;
    while (current) {
        print_log_info((LogInfo*)current->data);
        current = current->next;
    }
    
    list_free(head);
}

static void service_print_log(void) {
    char cardId[CARD_ID_MAX_LEN + 1];
    
    printf("\n日志打印管理\n");
    readCardId(cardId, sizeof(cardId));
    
    printf("\n--- 上下机记录 ---\n");
    ListNode* logHead = login_query_by_card(cardId);
    if (logHead) {
        ListNode* current = logHead;
        while (current) {
            print_log_info((LogInfo*)current->data);
            current = current->next;
        }
        list_free(logHead);
    } else {
        printf("暂无该卡的上下机记录。\n");
    }
    
    printf("\n--- 财务记录 ---\n");
    ListNode* finHead = finance_query_by_card(cardId);
    if (finHead) {
        ListNode* current = finHead;
        while (current) {
            print_finance_info((Finance*)current->data);
            current = current->next;
        }
        list_free(finHead);
    } else {
        printf("暂无该卡的财务记录。\n");
    }
}

static void service_query_by_date(void) {
    int year, month, day;
    int type;
    
    printf("\n按日期查询日志\n");
    printf("请选择查询类型 (1-上下机记录, 2-财务记录): ");
    scanf("%d", &type);
    clearInputBuffer();
    
    printf("请输入日期 (YYYY MM DD): ");
    scanf("%d %d %d", &year, &month, &day);
    clearInputBuffer();
    
    if (type == 1) {
        ListNode* head = login_query_by_date(year, month, day);
        if (!head) {
            printf("该日期暂无上下机记录。\n");
            return;
        }
        
        ListNode* current = head;
        while (current) {
            print_log_info((LogInfo*)current->data);
            current = current->next;
        }
        list_free(head);
    } else if (type == 2) {
        ListNode* head = finance_query_by_date(year, month, day);
        if (!head) {
            printf("该日期暂无财务记录。\n");
            return;
        }
        
        ListNode* current = head;
        while (current) {
            print_finance_info((Finance*)current->data);
            current = current->next;
        }
        list_free(head);
    } else {
        printf("无效类型。\n");
    }
}

void log_search_service(void) {
    const char* options[] = {
        "营业额日志",
        "上下机日志",
        "日志打印管理",
        "按日期查询日志",
        "返回上级"
    };
    
    while (1) {
        int choice = showMenu("查询统计", options, sizeof(options)/sizeof(options[0]));
        
        switch (choice) {
            case 0: service_revenue_log(); break;
            case 1: service_loginout_log(); break;
            case 2: service_print_log(); break;
            case 3: service_query_by_date(); break;
            case 4: return;
        }
    }
}
