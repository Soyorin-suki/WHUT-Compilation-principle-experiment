@echo off
REM tests\run_tests.bat — 在 Windows 下运行单个测试
REM 使用方式：在资源管理器双击或在 cmd 中运行 tests\run_tests.bat

SETLOCAL
REM %~dp0 是脚本所在目录（含末尾反斜杠）
SET SCRIPT_DIR=%~dp0

REM 确保 output 目录存在
IF NOT EXIST "%SCRIPT_DIR%output" (
    mkdir "%SCRIPT_DIR%output"
)

REM 可执行文件位于项目的 src\bin\lexing.exe（相对于 tests 目录的上级）

REM 定义输出文件路径并在运行前删除（避免追加或旧内容残留）
SET OUT_FILE=%SCRIPT_DIR%output\test1.out
IF EXIST "%OUT_FILE%" (
    del /f /q "%OUT_FILE%"
)

"%SCRIPT_DIR%..\src\bin\lexing.exe" -i "%SCRIPT_DIR%input\test1.txt" -o "%OUT_FILE%"

ENDLOCAL
