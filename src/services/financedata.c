#include "financedata.h"
#include "fileio.h"
#include "linkedlist.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

FinanceNode* finance_query_by_card(const char* cardId) {
    ListNode* head = file_read_all(FINANCE_FILE, FILE_MAGIC_FINANCE, sizeof(Finance));
    if (!head) return NULL;
    
    ListNode* result = NULL;
    ListNode* current = head;
    
    while (current) {
        Finance* f = (Finance*)current->data;
        if (strcmp(f->cardId, cardId) == 0) {
            result = list_append(result, current->data, sizeof(Finance));
        }
        current = current->next;
    }
    
    list_free(head);
    return result;
}

FinanceNode* finance_query_all(void) {
    return file_read_all(FINANCE_FILE, FILE_MAGIC_FINANCE, sizeof(Finance));
}

FinanceNode* finance_query_by_date(int year, int month, int day) {
    ListNode* head = file_read_all(FINANCE_FILE, FILE_MAGIC_FINANCE, sizeof(Finance));
    if (!head) return NULL;
    
    time_t startTime, endTime;
    build_date_range(year, month, day, &startTime, &endTime);
    
    /* 过滤该日期的记录 */
    ListNode* result = NULL;
    ListNode* current = head;
    
    while (current) {
        Finance* f = (Finance*)current->data;
        time_t recordTime = parseTime(f->time);
        if (recordTime >= startTime && recordTime < endTime) {
            result = list_append(result, current->data, sizeof(Finance));
        }
        current = current->next;
    }
    
    list_free(head);
    return result;
}

int finance_add(Finance finance) {
    /* 获取下一个ID */
    finance.id = file_get_next_finance_id(FINANCE_FILE);
    
    /* 获取当前时间 */
    getCurrentTime(finance.time, sizeof(finance.time));
    
    return file_append_record(FINANCE_FILE, FILE_MAGIC_FINANCE, &finance, sizeof(Finance));
}

float finance_statistics(int type) {
    ListNode* head = finance_query_all();
    if (!head) return 0.0f;
    
    float total = 0.0f;
    ListNode* current = head;
    
    while (current) {
        if (((Finance*)current->data)->type == type) {
            total += ((Finance*)current->data)->amount;
        }
        current = current->next;
    }
    
    list_free(head);
    return total;
}

int finance_get_next_id(void) {
    return file_get_next_finance_id(FINANCE_FILE);
}
