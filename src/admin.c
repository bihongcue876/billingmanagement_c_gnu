#include "admin.h"
#include "fileio.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>

static char current_admin[32] = {0};

static int verify_admin(const char* username, const char* password) {
    if (strcmp(username, "admin") == 0 && strcmp(password, "123456") == 0) {
        return 1;
    }
    return 0;
}

int admin_login(void) {
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
