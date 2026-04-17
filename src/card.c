#include "card.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static AccountNode* head = NULL;

void card_set_head(AccountNode* h) {
    head = h;
}

AccountNode* card_get_head(void) {
    return head;
}

AccountNode* card_find(const char* cardId) {
    AccountNode* p = head;
    while (p) {
        if (strcmp(p->data.aName, cardId) == 0 && p->data.nDel == 0) {
            return p;
        }
        p = p->next;
    }
    return NULL;
}

AccountNode* card_list(void) {
    AccountNode* p = head;
    AccountNode* result = NULL;
    AccountNode* tail = NULL;
    
    while (p) {
        if (p->data.nDel == 0) {
            AccountNode* node = (AccountNode*)malloc(sizeof(AccountNode));
            node->data = p->data;
            node->next = NULL;
            
            if (!result) {
                result = tail = node;
            } else {
                tail->next = node;
                tail = node;
            }
        }
        p = p->next;
    }
    return result;
}

int card_add(Account* acc) {
    if (card_find(acc->aName)) {
        return 0;
    }
    
    AccountNode* node = (AccountNode*)malloc(sizeof(AccountNode));
    node->data = *acc;
    node->next = NULL;
    
    if (!head) {
        head = node;
    } else {
        AccountNode* p = head;
        while (p->next) p = p->next;
        p->next = node;
    }
    return 1;
}

int card_delete(const char* cardId) {
    AccountNode* p = card_find(cardId);
    if (p) {
        p->data.nDel = 1;
        return 1;
    }
    return 0;
}

int card_update(Account* acc) {
    AccountNode* p = card_find(acc->aName);
    if (p) {
        p->data = *acc;
        return 1;
    }
    return 0;
}
