@echo off
chcp 65001 >nul
gcc -std=c99 -Wall -Wextra -Iinclude ^
    -finput-charset=UTF-8 -fexec-charset=GBK ^
    src\Main.c ^
    src\menu.c ^
    src\admin.c ^
    src\card.c ^
    src\fileio.c ^
    src\loginout.c ^
    src\finance.c ^
    src\crud.c ^
    src\utils.c ^
    -o app.exe -lws2_32
if %errorlevel% equ 0 (
    echo 编译成功！
    echo 运行: app.exe
)
