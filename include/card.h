#ifndef CARD_H
#define CARD_H

#include "model.h"

void card_set_head(AccountNode* h);
AccountNode* card_get_head(void);
AccountNode* card_find(const char* cardId);
AccountNode* card_list(void);
int card_add(Account* acc);
int card_delete(const char* cardId);
int card_update(Account* acc);
int card_search(const char* cardId);

#endif
