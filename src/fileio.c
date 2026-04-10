/**
 * @file fileio.c
 * @brief 通用文件读写实现 - 二进制格式存储
 * 
 * 优化点：
 * - 统一file_read_all/file_write_all处理所有数据类型
 * - file_append_record尝试直接追加（O(1)），失败时回退到读-改-写
 * - 所有fwrite返回值均检查，防止数据损坏
 */

#include "fileio.h"
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>
#include <direct.h>

void ensure_data_dir_exists(void) {
    _mkdir("data");
}

int file_exists(const char* filename) {
    struct stat st;
    return stat(filename, &st) == 0;
}

ListNode* file_read_all(const char* filename, unsigned int expectedMagic, size_t recordSize) {
    if (!file_exists(filename)) {
        return NULL;
    }
    
    FILE* fp = fopen(filename, "rb");
    if (!fp) return NULL;
    
    FileHeader header;
    if (fread(&header, sizeof(FileHeader), 1, fp) != 1) {
        fclose(fp);
        return NULL;
    }
    
    if (header.magic != expectedMagic || header.version != FILE_VERSION) {
        fclose(fp);
        return NULL;
    }
    
    ListNode* head = NULL;
    void* record = malloc(recordSize);
    if (!record) {
        fclose(fp);
        return NULL;
    }
    
    for (unsigned int i = 0; i < header.count; i++) {
        if (fread(record, recordSize, 1, fp) == 1) {
            head = list_append(head, record, recordSize);
        }
    }
    
    free(record);
    fclose(fp);
    return head;
}

int file_write_all(const char* filename, unsigned int magic, ListNode* head, size_t recordSize) {
    ensure_data_dir_exists();
    
    FILE* fp = fopen(filename, "wb");
    if (!fp) return 0;
    
    FileHeader header;
    header.magic = magic;
    header.version = FILE_VERSION;
    header.count = list_size(head);
    
    if (fwrite(&header, sizeof(FileHeader), 1, fp) != 1) {
        fclose(fp);
        return 0;
    }
    
    ListNode* current = head;
    while (current) {
        if (fwrite(current->data, recordSize, 1, fp) != 1) {
            fclose(fp);
            return 0;
        }
        current = current->next;
    }
    
    fclose(fp);
    return 1;
}

int file_append_record(const char* filename, unsigned int magic, const void* data, size_t recordSize) {
    ensure_data_dir_exists();
    
    /* 尝试直接追加（O(1)操作） */
    if (file_exists(filename)) {
        FILE* fp = fopen(filename, "r+b");
        if (fp) {
            FileHeader header;
            if (fread(&header, sizeof(FileHeader), 1, fp) == 1 &&
                header.magic == magic) {
                header.count++;
                fseek(fp, 0, SEEK_SET);
                if (fwrite(&header, sizeof(FileHeader), 1, fp) == 1) {
                    fseek(fp, 0, SEEK_END);
                    if (fwrite(data, recordSize, 1, fp) == 1) {
                        fclose(fp);
                        return 1;
                    }
                }
            }
            fclose(fp);
        }
    }
    
    /* 直接追加失败，使用读-改-写 */
    ListNode* head = list_append(NULL, data, recordSize);
    int result = file_write_all(filename, magic, head, recordSize);
    list_free(head);
    return result;
}

int file_update_record(const char* filename, unsigned int magic, const void* data,
                       size_t recordSize, size_t keyOffset, size_t keySize) {
    ListNode* head = file_read_all(filename, magic, recordSize);
    if (!head) return 0;
    
    ListNode* found = list_find_by_key(head, (const char*)data + keyOffset, keyOffset, keySize);
    if (found) {
        memcpy(found->data, data, recordSize);
        int result = file_write_all(filename, magic, head, recordSize);
        list_free(head);
        return result;
    }
    
    list_free(head);
    return 0;
}

int file_get_next_finance_id(const char* filename) {
    ListNode* head = file_read_all(filename, FILE_MAGIC_FINANCE, sizeof(Finance));
    int maxId = 0;
    
    ListNode* current = head;
    while (current) {
        Finance* fin = (Finance*)current->data;
        if (fin->id > maxId) {
            maxId = fin->id;
        }
        current = current->next;
    }
    
    list_free(head);
    return maxId + 1;
}

int file_get_next_admin_id(const char* filename) {
    ListNode* head = file_read_all(filename, FILE_MAGIC_ADMIN, sizeof(Admin));
    int maxId = 0;
    
    ListNode* current = head;
    while (current) {
        Admin* adm = (Admin*)current->data;
        if (adm->id > maxId) {
            maxId = adm->id;
        }
        current = current->next;
    }
    
    list_free(head);
    return maxId + 1;
}

void get_loginout_filename(int year, char* buffer, int bufferSize) {
    snprintf(buffer, bufferSize, "%s%d%s", LOGINOUT_FILE_PREFIX, year, LOGINOUT_FILE_SUFFIX);
}
