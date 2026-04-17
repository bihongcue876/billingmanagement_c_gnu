#include "fileio.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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

int save_billing(BillingStandard* standards, int count) {
    ensure_data_dir();
    FILE* fp = fopen(BILLING_FILE, "w");
    if (!fp) return 0;
    
    fprintf(fp, "%d\n", count);
    for (int i = 0; i < count; i++) {
        fprintf(fp, "%s %s %d %d\n", 
                standards[i].planId,
                standards[i].planName,
                standards[i].segmentCount,
                standards[i].nDel);
        for (int j = 0; j < standards[i].segmentCount; j++) {
            fprintf(fp, "%lf %lf %lf\n", 
                    standards[i].segments[j].startHour,
                    standards[i].segments[j].endHour,
                    standards[i].segments[j].pricePerHour);
        }
    }
    
    fclose(fp);
    return count;
}

int load_billing(BillingStandard* standards, int* count) {
    FILE* fp = fopen(BILLING_FILE, "r");
    if (!fp) return 0;
    
    if (fscanf(fp, "%d", count) != 1) {
        fclose(fp);
        return 0;
    }
    
    for (int i = 0; i < *count; i++) {
        fscanf(fp, "%s %s %d %d", 
               standards[i].planId,
               standards[i].planName,
               &standards[i].segmentCount,
               &standards[i].nDel);
        for (int j = 0; j < standards[i].segmentCount; j++) {
            fscanf(fp, "%lf %lf %lf", 
                   &standards[i].segments[j].startHour,
                   &standards[i].segments[j].endHour,
                   &standards[i].segments[j].pricePerHour);
        }
    }
    
    fclose(fp);
    return *count;
}

int save_admins(Admin* admins, int count) {
    ensure_data_dir();
    FILE* fp = fopen(ADMIN_FILE, "wb");
    if (!fp) return 0;
    
    fwrite(&count, sizeof(int), 1, fp);
    fwrite(admins, sizeof(Admin), count, fp);
    
    fclose(fp);
    return count;
}

int load_admins(Admin* admins, int* count) {
    FILE* fp = fopen(ADMIN_FILE, "rb");
    if (!fp) return 0;
    
    fread(count, sizeof(int), 1, fp);
    fread(admins, sizeof(Admin), *count, fp);
    
    fclose(fp);
    return *count;
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

void log_query(const char* queryType, const char* cardId, const char* result) {
    time_t now = time(NULL);
    struct tm* t = localtime(&now);
    char timeStr[64];
    strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", t);
    
    char logLine[512];
    sprintf(logLine, "[%s] QueryType: %s, Card: %s, Result: %s\n", 
            timeStr, queryType, cardId, result);
    
    printf("%s", logLine);
    
    if (strstr(queryType, "UseLog") == NULL && strstr(queryType, "Sales") == NULL) {
        return;
    }
    
    char logFile[64];
    int year = t->tm_year + 1900;
    if (strstr(queryType, "UseLog") != NULL) {
        sprintf(logFile, "uselog%d.log", year);
    } else {
        sprintf(logFile, "sales%d.log", year);
    }
    
    FILE* fp = fopen(logFile, "a");
    if (!fp) return;
    
    fprintf(fp, "%s", logLine);
    
    fclose(fp);
}
