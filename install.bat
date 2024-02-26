@echo off
:: Check if the script is running as Administrator
net session >nul 2>&1
if %errorLevel% == 0 (
    echo Running with administrative privileges
    goto Start
) else (
    echo Requesting administrative privileges...
)

:UACPrompt
    echo Set UAC = CreateObject^("Shell.Application"^) > "%temp%\getadmin.vbs"
    echo UAC.ShellExecute "cmd.exe", "/c ""%~s0""", "", "runas", 1 >> "%temp%\getadmin.vbs"
    cscript //nologo "%temp%\getadmin.vbs"
    del "%temp%\getadmin.vbs"
    exit /B

:Start
REM Define the source file and the output executable name
set "SOURCE_FILE=main.cpp"
set "EXECUTABLE_NAME=prompter.exe"

REM Compile the source file
g++ -o %EXECUTABLE_NAME% %SOURCE_FILE%
if %ERRORLEVEL% NEQ 0 (
    echo Compilation failed.
    exit /b 1
)

REM Move the executable to C:\Windows\System32
move /Y %EXECUTABLE_NAME% C:\Windows\System32\
if %ERRORLEVEL% NEQ 0 (
    echo Failed to move the executable to C:\Windows\System32.
    exit /b 1
)

echo Installation successful.
