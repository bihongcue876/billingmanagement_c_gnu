#include "fileio.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void ensure_data_dir(void) {
    system("if not exist data mkdir data");
}

int save_accounts(AccountNode* head) {
    ensure_data_dir();
    FILE* fp = fopen(ACCOUNT_FILE, "wb");
    if (!fp) return 0;
    
    int count = 0;
    AccountNode* p = head;
    while (p) {
        if (p->data.nDel == 0) {
            fwrite(&p->data, sizeof(Account), 1, fp);
            count++;
        }
        p = p->next;
    }
    
    fclose(fp);
    return count;
}

AccountNode* load_accounts(void) {
    FILE* fp = fopen(ACCOUNT_FILE, "rb");
    if (!fp) return NULL;
    
    AccountNode* head = NULL;
    AccountNode* tail = NULL;
    Account acc;
    
    while (fread(&acc, sizeof(Account), 1, fp) == 1) {
        AccountNode* node = (AccountNode*)malloc(sizeof(AccountNode));
        node->data = acc;
        node->next = NULL;
        
        if (!head) {
            head = tail = node;
        } else {
            tail->next = node;
            tail = node;
        }
    }
    
    fclose(fp);
    return head;
}

int save_finance(FinanceNode* head) {
    ensure_data_dir();
    FILE* fp = fopen(FINANCE_FILE, "wb");
    if (!fp) return 0;
    
    FinanceNode* p = head;
    while (p) {
        fwrite(&p->data, sizeof(Finance), 1, fp);
        p = p->next;
    }
    
    fclose(fp);
    return 1;
}

FinanceNode* load_finance(void) {
    FILE* fp = fopen(FINANCE_FILE, "rb");
    if (!fp) return NULL;
    
    FinanceNode* head = NULL;
    FinanceNode* tail = NULL;
    Finance fin;
    
    while (fread(&fin, sizeof(Finance), 1, fp) == 1) {
        FinanceNode* node = (FinanceNode*)malloc(sizeof(FinanceNode));
        node->data = fin;
        node->next = NULL;
        
        if (!head) {
            head = tail = node;
        } else {
            tail->next = node;
            tail = node;
        }
    }
    
    fclose(fp);
    return head;
}

int append_loginfo(LogInfo* info) {
    ensure_data_dir();
    FILE* fp = fopen(LOGINOUT_FILE, "ab");
    if (!fp) return 0;
    
    fwrite(info, sizeof(LogInfo), 1, fp);
    fclose(fp);
    return 1;
}

LogInfoNode* load_loginfo(void) {
    FILE* fp = fopen(LOGINOUT_FILE, "rb");
    if (!fp) return NULL;
    
    LogInfoNode* head = NULL;
    LogInfoNode* tail = NULL;
    LogInfo log;
    
    while (fread(&log, sizeof(LogInfo), 1, fp) == 1) {
        LogInfoNode* node = (LogInfoNode*)malloc(sizeof(LogInfoNode));
        node->data = log;
        node->next = NULL;
        
        if (!head) {
            head = tail = node;
        } else {
            tail->next = node;
            tail = node;
        }
    }
    
    fclose(fp);
    return head;
}
