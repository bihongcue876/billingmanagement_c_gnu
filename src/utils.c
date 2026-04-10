/**
 * @file utils.c
 * @brief 通用工具函数实现
 */

#include "utils.h"
#include "model.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <conio.h>
#include <ctype.h>
#include <time.h>

void clearInputBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

float readFloat(const char* prompt, float minVal, float maxVal) {
    float value;
    while (1) {
        printf("%s", prompt);
        if (scanf("%f", &value) == 1) {
            clearInputBuffer();
            if (value >= minVal && value <= maxVal) {
                return value;
            }
            printf("输入超出范围 [%.2f - %.2f]，请重新输入。\n", minVal, maxVal);
        } else {
            clearInputBuffer();
            printf("输入无效，请输入数字。\n");
        }
    }
}

int readInt(const char* prompt) {
    int value;
    while (1) {
        printf("%s", prompt);
        if (scanf("%d", &value) == 1) {
            clearInputBuffer();
            return value;
        } else {
            clearInputBuffer();
            printf("输入无效，请输入整数。\n");
        }
    }
}

char readOption(const char* prompt) {
    char option;
    printf("%s", prompt);
    option = _getch();
    printf("%c\n", option);
    return option;
}

void readPassword(char* password, int maxSize) {
    int i = 0;
    char ch;
    
    while (i < maxSize - 1) {
        ch = _getch();
        if (ch == '\r' || ch == '\n') {
            break;
        } else if (ch == '\b') {
            if (i > 0) {
                i--;
                printf("\b \b");
            }
        } else if (isprint(ch)) {
            password[i++] = ch;
            printf("*");
        }
    }
    password[i] = '\0';
    printf("\n");
}

void readCardId(char* cardId, int maxSize) {
    char input[256];
    int valid;
    
    while (1) {
        printf("请输入卡号 (字母/数字/下划线, 最多%d位): ", MAX_CARD_ID_INPUT_LEN);
        if (scanf("%255s", input) == 1) {
            clearInputBuffer();
            
            if (strlen(input) > MAX_CARD_ID_INPUT_LEN || strlen(input) == 0) {
                printf("卡号长度必须在1-%d位之间。\n", MAX_CARD_ID_INPUT_LEN);
                continue;
            }
            
            valid = 1;
            for (int i = 0; input[i]; i++) {
                if (!isalnum(input[i]) && input[i] != '_') {
                    valid = 0;
                    break;
                }
            }
            
            if (valid) {
                strncpy(cardId, input, maxSize - 1);
                cardId[maxSize - 1] = '\0';
                return;
            } else {
                printf("卡号只能包含字母、数字和下划线。\n");
            }
        } else {
            clearInputBuffer();
            printf("输入无效。\n");
        }
    }
}

int readOptionalCardId(char* cardId, int maxSize) {
    printf("请输入卡号 (留空查询全部): ");
    
    int ch = getchar();
    if (ch == '\n') {
        cardId[0] = '\0';
        return 0;
    }
    
    ungetc(ch, stdin);
    readCardId(cardId, maxSize);
    return 1;
}

int showMenu(const char* title, const char** options, int optionCount) {
    int choice;
    
    printf("\n-----------------------------------\n");
    printf("%s\n", title);
    printf("-----------------------------------\n");
    
    for (int i = 0; i < optionCount; i++) {
        printf("%d. %s\n", i, options[i]);
    }
    printf("-----------------------------------\n");
    
    while (1) {
        printf("请选择: ");
        if (scanf("%d", &choice) == 1) {
            clearInputBuffer();
            if (choice >= 0 && choice < optionCount) {
                return choice;
            }
        } else {
            clearInputBuffer();
        }
        printf("无效选项，请重新选择。\n");
    }
}

void showSeparator(void) {
    printf("-----------------------------------\n");
}

void formatTime(time_t t, char* buffer, int bufferSize) {
    struct tm* tmInfo = localtime(&t);
    strftime(buffer, bufferSize, "%Y-%m-%d %H:%M:%S", tmInfo);
}

time_t parseTime(const char* timeStr) {
    struct tm tmInfo = {0};
    int year, month, day, hour = 0, minute = 0, second = 0;
    
    if (sscanf(timeStr, "%d-%d-%d %d:%d:%d", &year, &month, &day, &hour, &minute, &second) >= 3) {
        tmInfo.tm_year = year - 1900;
        tmInfo.tm_mon = month - 1;
        tmInfo.tm_mday = day;
        tmInfo.tm_hour = hour;
        tmInfo.tm_min = minute;
        tmInfo.tm_sec = second;
        tmInfo.tm_isdst = -1;
        return mktime(&tmInfo);
    }
    return 0;
}

void getCurrentTime(char* buffer, int bufferSize) {
    time_t now = time(NULL);
    formatTime(now, buffer, bufferSize);
}

void build_date_range(int year, int month, int day, time_t* startTime, time_t* endTime) {
    struct tm startTm = {0};
    startTm.tm_year = year - 1900;
    startTm.tm_mon = month - 1;
    startTm.tm_mday = day;
    startTm.tm_hour = 0;
    startTm.tm_min = 0;
    startTm.tm_sec = 0;
    startTm.tm_isdst = -1;
    *startTime = mktime(&startTm);
    
    struct tm endTm = startTm;
    endTm.tm_mday += 1;
    *endTime = mktime(&endTm);
}
