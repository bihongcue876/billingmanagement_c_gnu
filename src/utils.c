#include "utils.h"
#include <stdio.h>
#include <string.h>
#include <conio.h>

int get_int(const char* prompt) {
    int value;
    printf("%s", prompt);
    scanf("%d", &value);
    getchar();
    return value;
}

float get_float(const char* prompt) {
    float value;
    printf("%s", prompt);
    scanf("%f", &value);
    getchar();
    return value;
}

void get_string(const char* prompt, char* buffer, int size) {
    printf("%s", prompt);
    fgets(buffer, size, stdin);
    buffer[strcspn(buffer, "\n")] = '\0';
}

int get_choice(int min, int max) {
    int choice;
    printf("请选择 (%d-%d): ", min, max);
    scanf("%d", &choice);
    getchar();
    if (choice < min || choice > max) {
        return -1;
    }
    return choice;
}

void get_password(const char* prompt, char* buffer, int size) {
    printf("%s", prompt);
    int i = 0;
    char ch;
    
    while ((ch = _getch()) != '\r') {
        if (ch == '\b') {
            if (i > 0) {
                i--;
                printf("\b \b");
            }
        } else if (i < size - 1) {
            buffer[i++] = ch;
            printf("*");
        }
    }
    buffer[i] = '\0';
    printf("\n");
}
