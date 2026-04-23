@echo off
chcp 65001 >nul
setlocal enabledelayedexpansion

echo ========================================
echo 批量特征识别处理
echo ========================================
echo.

rem 创建输出目录
if not exist "output" (
    mkdir output
    echo [信息] 创建输出目录: output
)

rem 统计文件总数
set /a total=0
for %%f in (data\*.stp data\*.step) do (
    set /a total+=1
)

if %total% equ 0 (
    echo [错误] 未找到任何 .stp 或 .step 文件
    pause
    exit /b 1
)

echo [信息] 找到 %total% 个模型文件
echo.

rem 处理文件
set /a current=0
set /a success=0
set /a failed=0

for %%f in (data\*.stp data\*.step) do (
    set /a current+=1

    rem 获取不带扩展名的文件名
    set "filename=%%~nf"
    set "input_file=%%f"
    set "output_file=Features\!filename!.json"

    echo [!current!/%total%] 处理: %%~nxf

    rem 调用识别程序
    .\standalone\CNC_FeatureRecognizer.exe "!input_file!" "!output_file!" >nul 2>&1

    if !errorlevel! equ 0 (
        echo          ✓ 成功生成: !output_file!
        set /a success+=1
    ) else (
        echo          × 失败: %%~nxf
        set /a failed+=1
    )
    echo.
)

echo ========================================
echo 处理完成
echo ========================================
echo 总计: %total% 个文件
echo 成功: %success% 个
echo 失败: %failed% 个
echo ========================================
pause
