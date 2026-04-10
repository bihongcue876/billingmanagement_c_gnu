/*
 * utils.h - 通用工具函数
 */

#ifndef UTILS_H
#define UTILS_H

#include <time.h>

void clearInputBuffer(void);
float readFloat(const char* prompt, float minVal, float maxVal);
int readInt(const char* prompt);
char readOption(const char* prompt);
void readPassword(char* password, int maxSize);
void readCardId(char* cardId, int maxSize);

/* 读取可选卡号，留空返回0 */
int readOptionalCardId(char* cardId, int maxSize);

int showMenu(const char* title, const char** options, int optionCount);
void showSeparator(void);
void formatTime(time_t t, char* buffer, int bufferSize);
time_t parseTime(const char* timeStr);
void getCurrentTime(char* buffer, int bufferSize);

/* 构建日期范围（当天00:00:00到次日00:00:00） */
void build_date_range(int year, int month, int day, time_t* startTime, time_t* endTime);

#endif /* UTILS_H */
