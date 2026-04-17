#ifndef LOGINOUT_H
#define LOGINOUT_H

#include "model.h"

int login_on(const char* cardId, const char* password);
int login_off(const char* cardId);

#endif
