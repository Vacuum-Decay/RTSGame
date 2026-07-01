@echo Started: %date% %time%
@echo off
echo Compiling...
g++ -g main.cpp -o app.exe -mwindows -lgdi32

if %errorlevel% equ 0 (
    echo Success! Running app...
    start app.exe
) else (
    echo Compilation failed.
    pause
)
@echo on
@echo Ended: %date% %time%