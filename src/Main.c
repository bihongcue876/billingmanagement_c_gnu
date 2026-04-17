#include "menu.h"
#include "admin.h"
#include "utils.h"
#include <stdio.h>

int main(void) {
    while (1) {
        printf("\n---------------------------\n");
        printf("     计费管理系统\n");
        printf("---------------------------\n");
        printf("  1. 登录\n");
        printf("  0. 退出系统\n");
        printf("---------------------------\n");
        printf("请输入选项：");
        
        int choice = get_choice(0, 1);
        
        if (choice == 1) {
            int status = admin_login();
            if (!status) {
                printf("\n登录失败，返回主菜单\n");
                continue;
            }
            main_menu();
            printf("\n已登出，请重新登录或退出系统\n");
        } else if (choice == 0) {
            printf("\n正在退出系统...\n");
            break;
        }
    }
    
    return 0;
}
