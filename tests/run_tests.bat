@echo off
REM tests\run_tests.bat — 依序运行 input 中的所有 .txt，并输出到 output
REM 使用方式：双击或在 cmd 中运行本脚本

SETLOCAL ENABLEDELAYEDEXPANSION

REM 定位目录与可执行文件
SET "SCRIPT_DIR=%~dp0"
SET "INPUT_DIR=%SCRIPT_DIR%input"
SET "OUTPUT_DIR=%SCRIPT_DIR%output"
SET "EXE=%SCRIPT_DIR%..\src\bin\lexing.exe"

REM 确保 output 目录存在
IF NOT EXIST "%OUTPUT_DIR%" (
    mkdir "%OUTPUT_DIR%"
)

REM 清理旧的 .out 文件
IF EXIST "%OUTPUT_DIR%\*.out" (
    del /f /q "%OUTPUT_DIR%\*.out"
)

REM 检查可执行文件是否存在
IF NOT EXIST "%EXE%" (
        echo 未找到可执行文件：%EXE%
        echo 请先编译项目（在 VS Code 中运行构建任务或手动用 g++ 编译）。
    exit /b 1
)

REM 按文件名排序遍历所有 .txt 输入
SET /A idx=1
FOR /F "delims=" %%F IN ('dir /b /on "%INPUT_DIR%\*.txt"') DO (
    SET "name=%%~nF"
    SET "infile=%INPUT_DIR%\%%~nxF"
    SET "outfile=%OUTPUT_DIR%\!name!.out"
    echo [!idx!] 处理 %%F ^> !outfile!
    "%EXE%" -i "!infile!" -o "!outfile!"
    IF ERRORLEVEL 1 (
        echo 发生错误，停止于：%%F
        exit /b 1
    )
    SET /A idx+=1
)

echo 全部测试完成。
ENDLOCAL
