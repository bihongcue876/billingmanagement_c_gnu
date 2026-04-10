/*
 * fileio.h - 通用文件读写接口
 * 文件格式: FileHeader(12字节) + N条记录
 */

#ifndef FILEIO_H
#define FILEIO_H

#include "model.h"
#include "linkedlist.h"
#include <stdio.h>

/* 文件魔数标识 */
#define FILE_MAGIC_ACCOUNT 0x41434354  /* "ACCT" */
#define FILE_MAGIC_BILLING 0x424C4C4E  /* "BLLN" */
#define FILE_MAGIC_LOGINFO 0x4C4F4749  /* "LOGI" */
#define FILE_MAGIC_FINANCE 0x46494E4E  /* "FINN" */
#define FILE_MAGIC_ADMIN   0x41444D4E  /* "ADMN" */
#define FILE_VERSION 1

/* 数据文件路径 */
#define DATA_DIR "data/"
#define ADMIN_FILE DATA_DIR "admins.bin"
#define ACCOUNT_FILE DATA_DIR "accounts.bin"
#define BILLING_FILE DATA_DIR "billings.bin"
#define FINANCE_FILE DATA_DIR "finance.bin"
#define LOGINOUT_FILE_PREFIX DATA_DIR "loginout_"
#define LOGINOUT_FILE_SUFFIX ".bin"

/* 计费参数常量 */
#define DEFAULT_PACKAGE_ID "0"
#define DEFAULT_UNIT_PRICE 0.1f
#define DEFAULT_UNIT_TYPE UNIT_MINUTE

/* 文件头结构 */
typedef struct {
    unsigned int magic;    /* 魔数标识 */
    unsigned int version;  /* 版本号 */
    unsigned int count;    /* 记录数量 */
} FileHeader;

/* 通用文件操作 */
void ensure_data_dir_exists(void);
int file_exists(const char* filename);
ListNode* file_read_all(const char* filename, unsigned int expectedMagic, size_t recordSize);
int file_write_all(const char* filename, unsigned int magic, ListNode* head, size_t recordSize);
int file_append_record(const char* filename, unsigned int magic, const void* data, size_t recordSize);
int file_update_record(const char* filename, unsigned int magic, const void* data, 
                       size_t recordSize, size_t keyOffset, size_t keySize);

/* 特定操作 */
int file_get_next_finance_id(const char* filename);
int file_get_next_admin_id(const char* filename);
void get_loginout_filename(int year, char* buffer, int bufferSize);

#endif /* FILEIO_H */
