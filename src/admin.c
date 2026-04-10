#include "admin.h"
#include "fileio.h"
#include "linkedlist.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stddef.h>

void admin_init_defaults(void) {
    if (!file_exists(ADMIN_FILE)) {
        Admin defaultAdmin;
        defaultAdmin.id = 1;
        strncpy(defaultAdmin.username, "admin", sizeof(defaultAdmin.username) - 1);
        defaultAdmin.username[sizeof(defaultAdmin.username) - 1] = '\0';
        strncpy(defaultAdmin.password, "123456", sizeof(defaultAdmin.password) - 1);
        defaultAdmin.password[sizeof(defaultAdmin.password) - 1] = '\0';
        getCurrentTime(defaultAdmin.createTime, sizeof(defaultAdmin.createTime));
        
        file_append_record(ADMIN_FILE, FILE_MAGIC_ADMIN, &defaultAdmin, sizeof(Admin));
        printf("已初始化默认管理员账号: admin / 123456\n");
    }
}

int admin_login(Admin* admin) {
    char username[32];
    char password[32];
    int attempts = 0;
    
    printf("\n-----------------------------------\n");
    printf("管理员登录\n");
    printf("-----------------------------------\n");
    
    while (attempts < MAX_LOGIN_ATTEMPTS) {
        printf("用户名: ");
        if (scanf("%31s", username) != 1) {
            clearInputBuffer();
            printf("输入无效。\n");
            continue;
        }
        clearInputBuffer();
        
        printf("密码: ");
        readPassword(password, sizeof(password));
        
        /* 查找管理员 */
        ListNode* head = file_read_all(ADMIN_FILE, FILE_MAGIC_ADMIN, sizeof(Admin));
        ListNode* found = NULL;
        ListNode* current = head;
        while (current) {
            if (strcmp(((Admin*)current->data)->username, username) == 0) {
                found = current;
                break;
            }
            current = current->next;
        }

        if (found && strcmp(((Admin*)found->data)->password, password) == 0) {
            memcpy(admin, found->data, sizeof(Admin));
            list_free(head);
            printf("登录成功！欢迎 %s\n", admin->username);
            return 1;
        }
        
        list_free(head);
        attempts++;
        
        if (attempts < MAX_LOGIN_ATTEMPTS) {
            printf("用户名或密码错误！还剩 %d 次机会。\n", MAX_LOGIN_ATTEMPTS - attempts);
        }
    }
    
    printf("失败次数过多，已返回。\n");
    return 0;
}
