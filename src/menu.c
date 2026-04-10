#include "menu.h"
#include "service.h"
#include "utils.h"
#include <stdio.h>

void main_menu(void) {
    const char* options[] = {
        "管理员管理",
        "账户服务",
        "上下机服务",
        "计费标准管理",
        "财务服务",
        "查询统计",
        "退出系统"
    };
    
    while (1) {
        printf("\n===================================\n");
        printf("       计费管理系统 - 主菜单\n");
        printf("===================================\n");
        
        int choice = showMenu("请选择功能", options, sizeof(options)/sizeof(options[0]));
        
        if (choice == 6) {
            printf("感谢使用，再见！\n");
            break;
        }
        
        /* choice 0-5 对应 service_dispatch 1-6 */
        service_dispatch(choice + 1);
    }
}
