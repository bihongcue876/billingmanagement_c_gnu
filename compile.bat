@echo off
chcp 65001 >nul
gcc -std=c99 -Wall -Wextra -Iinclude ^
    -finput-charset=UTF-8 -fexec-charset=GBK ^
    src\Main.c ^
    src\utils.c ^
    src\linkedlist.c ^
    src\fileio.c ^
    src\admin.c ^
    src\menu.c ^
    src\service.c ^
    src\adminservice.c ^
    src\accountdata.c ^
    src\services\accountservice.c ^
    src\services\billingdatabase.c ^
    src\services\billingstandard.c ^
    src\services\logindata.c ^
    src\services\loginout.c ^
    src\services\financedata.c ^
    src\services\financeservice.c ^
    src\services\logsearch.c ^
    -o app.exe -lws2_32
if %errorlevel% equ 0 (
    echo 编译成功！
    echo 运行: app.exe
)
