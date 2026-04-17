#include "admin.h"
#include "fileio.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

static char current_admin[32] = {0};
static Admin admins[10];
static int admin_count = 0;

static void load_admins_list(void) {
    admin_count = 0;
    load_admins(admins, &admin_count);
    if (admin_count == 0) {
        strcpy(admins[0].username, "admin");
        strcpy(admins[0].password, "123456");
        strcpy(admins[0].createTime, "2024-01-01");
        admin_count = 1;
        save_admins(admins, admin_count);
    }
}

static int verify_admin(const char* username, const char* password) {
    load_admins_list();
    for (int i = 0; i < admin_count; i++) {
        if (strcmp(admins[i].username, username) == 0 && 
            strcmp(admins[i].password, password) == 0) {
            return 1;
        }
    }
    return 0;
}

int admin_login(void) {
    load_admins_list();
    char username[32];
    char password[32];
    int attempts = 0;
    
    while (attempts < 5) {
        printf("\n=== 管理员登录 ===\n");
        get_string("请输入管理员账号: ", username, sizeof(username));
        get_password("请输入密码: ", password, sizeof(password));
        
        if (verify_admin(username, password)) {
            strcpy(current_admin, username);
            printf("登录成功！\n");
            return 1;
        }
        
        attempts++;
        printf("账号或密码错误，还剩 %d 次机会\n", 5 - attempts);
    }
    
    printf("登录失败次数过多，退出系统\n");
    return 0;
}

void admin_logout(void) {
    current_admin[0] = '\0';
}

void admin_save(void) {
    save_admins(admins, admin_count);
}

void admin_menu(void) {
    load_admins_list();
    
    while (1) {
        printf("\n=== 管理员管理 ===\n");
        printf("1. 查看管理员列表\n");
        printf("2. 添加管理员\n");
        printf("3. 删除管理员\n");
        printf("4. 修改密码\n");
        printf("0. 返回\n");
        
        int choice = get_choice(0, 4);
        if (choice == 0) break;
        
        char username[32];
        char password[32];
        
        switch (choice) {
            case 1:
                printf("\n=== 管理员列表 ===\n");
                printf("用户名\t\t创建时间\n");
                for (int i = 0; i < admin_count; i++) {
                    printf("%s\t\t%s\n", admins[i].username, admins[i].createTime);
                }
                printf("共 %d 个管理员\n", admin_count);
                break;
                
            case 2:
                printf("\n添加管理员\n");
                if (admin_count >= 10) {
                    printf("管理员已满！\n");
                    break;
                }
                get_string("请输入新管理员账号: ", username, sizeof(username));
                {
                    int exists = 0;
                    for (int i = 0; i < admin_count; i++) {
                        if (strcmp(admins[i].username, username) == 0) {
                            exists = 1;
                            break;
                        }
                    }
                    if (exists) {
                        printf("管理员已存在！\n");
                        break;
                    }
                }
                get_string("请输入密码: ", password, sizeof(password));
                
                strcpy(admins[admin_count].username, username);
                strcpy(admins[admin_count].password, password);
                time_t now = time(NULL);
                struct tm* t = localtime(&now);
                sprintf(admins[admin_count].createTime, "%04d-%02d-%02d", 
                       t->tm_year + 1900, t->tm_mon + 1, t->tm_mday);
                admin_count++;
                
                save_admins(admins, admin_count);
                printf("添加成功！\n");
                break;
                
            case 3:
                printf("\n删除管理员\n");
                if (admin_count <= 1) {
                    printf("不能删除最后一个管理员！\n");
                    break;
                }
                get_string("请输入要删除的管理员账号: ", username, sizeof(username));
                
                if (strcmp(username, current_admin) == 0) {
                    printf("不能删除当前登录的管理员！\n");
                    break;
                }
                
                {
                    int found = -1;
                    for (int i = 0; i < admin_count; i++) {
                        if (strcmp(admins[i].username, username) == 0) {
                            found = i;
                            break;
                        }
                    }
                    if (found == -1) {
                        printf("管理员不存在！\n");
                    } else {
                        for (int i = found; i < admin_count - 1; i++) {
                            admins[i] = admins[i + 1];
                        }
                        admin_count--;
                        save_admins(admins, admin_count);
                        printf("删除成功！\n");
                    }
                }
                break;
                
            case 4:
                {
                    char oldPwd[32], newPwd[32];
                    printf("\n修改密码\n");
                    get_password("请输入原密码: ", oldPwd, sizeof(oldPwd));
                    {
                        int found = -1;
                        for (int i = 0; i < admin_count; i++) {
                            if (strcmp(admins[i].username, current_admin) == 0) {
                                found = i;
                                break;
                            }
                        }
                        if (found == -1 || strcmp(admins[found].password, oldPwd) != 0) {
                            printf("原密码错误！\n");
                            break;
                        }
                        get_password("请输入新密码: ", newPwd, sizeof(newPwd));
                        strcpy(admins[found].password, newPwd);
                        save_admins(admins, admin_count);
                        printf("密码修改成功！\n");
                    }
                }
                break;
        }
    }
}
