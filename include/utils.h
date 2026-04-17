#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>

int get_int(const char* prompt);
float get_float(const char* prompt);
void get_string(const char* prompt, char* buffer, int size);
int get_choice(int min, int max);
void get_password(const char* prompt, char* buffer, int size);

#endif
