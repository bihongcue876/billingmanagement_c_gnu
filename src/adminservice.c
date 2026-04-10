#include "adminservice.h"
#include "admin.h"
#include "fileio.h"
#include "linkedlist.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stddef.h>

static void service_change_password(Admin* admin) {
    char oldPwd[32];
    char newPwd[32];
    
    printf("\n修改密码\n");
    printf("当前密码: ");
    readPassword(oldPwd, sizeof(oldPwd));
    
    if (strcmp(admin->password, oldPwd) != 0) {
        printf("当前密码错误！\n");
        return;
    }
    
    printf("新密码: ");
    readPassword(newPwd, sizeof(newPwd));
    
    strncpy(admin->password, newPwd, sizeof(admin->password) - 1);
    admin->password[sizeof(admin->password) - 1] = '\0';
    
    if (file_update_record(ADMIN_FILE, FILE_MAGIC_ADMIN, admin, sizeof(Admin),
                           offsetof(Admin, username), USERNAME_MAX_LEN + 1)) {
        printf("密码修改成功！\n");
    } else {
        printf("密码修改失败。\n");
    }
}

static void service_add_admin(void) {
    Admin newAdmin;
    char username[32];
    char password[32];
    
    printf("\n添加管理员\n");
    printf("用户名: ");
    if (scanf("%31s", username) != 1) {
        clearInputBuffer();
        printf("输入无效。\n");
        return;
    }
    clearInputBuffer();
    
    /* 一次性读取完成检查存在性和计算maxId */
    ListNode* head = file_read_all(ADMIN_FILE, FILE_MAGIC_ADMIN, sizeof(Admin));
    int maxId = 0;
    int exists = 0;
    
    if (head) {
        ListNode* current = head;
        while (current) {
            Admin* adm = (Admin*)current->data;
            if (strcmp(adm->username, username) == 0) {
                exists = 1;
            }
            if (adm->id > maxId) {
                maxId = adm->id;
            }
            current = current->next;
        }
    }
    
    if (exists) {
        printf("该用户名已存在。\n");
        list_free(head);
        return;
    }
    
    strncpy(newAdmin.username, username, sizeof(newAdmin.username) - 1);
    newAdmin.username[sizeof(newAdmin.username) - 1] = '\0';
    
    printf("密码: ");
    readPassword(password, sizeof(password));
    strncpy(newAdmin.password, password, sizeof(newAdmin.password) - 1);
    newAdmin.password[sizeof(newAdmin.password) - 1] = '\0';
    
    newAdmin.id = maxId + 1;
    getCurrentTime(newAdmin.createTime, sizeof(newAdmin.createTime));
    
    list_free(head);
    
    if (file_append_record(ADMIN_FILE, FILE_MAGIC_ADMIN, &newAdmin, sizeof(Admin))) {
        printf("管理员添加成功！\n");
    } else {
        printf("管理员添加失败。\n");
    }
}

static void service_delete_admin(Admin* currentAdmin) {
    char username[32];
    
    printf("\n删除管理员\n");
    printf("用户名: ");
    if (scanf("%31s", username) != 1) {
        clearInputBuffer();
        printf("输入无效。\n");
        return;
    }
    clearInputBuffer();
    
    if (strcmp(username, currentAdmin->username) == 0) {
        printf("不能删除当前登录的管理员账号。\n");
        return;
    }
    
    ListNode* head = file_read_all(ADMIN_FILE, FILE_MAGIC_ADMIN, sizeof(Admin));
    ListNode* found = list_find_by_key(head, username, offsetof(Admin, username), USERNAME_MAX_LEN + 1);
    
    if (!found) {
        printf("未找到该管理员。\n");
        list_free(head);
        return;
    }
    
    printf("确认删除管理员 '%s'？(y/n): ", username);
    char choice = readOption("");
    
    if (choice == 'y' || choice == 'Y') {
        head = list_remove_by_key(head, username, offsetof(Admin, username), USERNAME_MAX_LEN + 1);
        if (file_write_all(ADMIN_FILE, FILE_MAGIC_ADMIN, head, sizeof(Admin))) {
            printf("管理员删除成功！\n");
        } else {
            printf("管理员删除失败。\n");
        }
    } else {
        printf("已取消操作。\n");
    }
    
    list_free(head);
}

static void service_list_admin(void) {
    printf("\n-----------------------------------\n");
    printf("管理员列表\n");
    printf("-----------------------------------\n");
    
    ListNode* head = file_read_all(ADMIN_FILE, FILE_MAGIC_ADMIN, sizeof(Admin));
    if (!head) {
        printf("暂无管理员。\n");
        return;
    }
    
    ListNode* current = head;
    while (current) {
        Admin* adm = (Admin*)current->data;
        showSeparator();
        printf("ID: %d\n", adm->id);
        printf("用户名: %s\n", adm->username);
        printf("创建时间: %s\n", adm->createTime);
        current = current->next;
    }
    
    showSeparator();
    printf("总计: %d 个管理员\n", list_size(head));
    list_free(head);
}

void admin_service(void) {
    Admin admin;
    
    if (!admin_login(&admin)) {
        return;
    }
    
    const char* options[] = {
        "修改密码",
        "添加管理员",
        "删除管理员",
        "管理员列表",
        "返回上级"
    };
    
    while (1) {
        int choice = showMenu("管理员管理", options, sizeof(options)/sizeof(options[0]));
        
        switch (choice) {
            case 0: service_change_password(&admin); break;
            case 1: service_add_admin(); break;
            case 2: service_delete_admin(&admin); break;
            case 3: service_list_admin(); break;
            case 4: return;
        }
    }
}
