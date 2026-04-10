/*
 * Main.c - 计费管理系统主程序入口
 */

#include "menu.h"
#include "admin.h"
#include <stdio.h>
#include <locale.h>

int main(void) {
    setlocale(LC_ALL, "");

    printf("===================================\n");
    printf("   计费管理系统 v1.0 (C语言版)\n");
    printf("===================================\n");

    admin_init_defaults();

    main_menu();

    return 0;
}
