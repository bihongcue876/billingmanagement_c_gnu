/*
 * admin.h - 管理员登录接口
 */

#ifndef ADMIN_H
#define ADMIN_H

#include "model.h"

/* 管理员登录（返回1成功，0失败） */
int admin_login(Admin* admin);

/* 初始化默认管理员账号 */
void admin_init_defaults(void);

#endif /* ADMIN_H */
