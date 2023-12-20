@echo off
cls

echo ===========================================
echo Building the writer program...
echo ===========================================
gcc -o writer.exe writer.c -lwinmm
if %ERRORLEVEL% neq 0 (
    echo Failed to compile writer.c
    exit /b %ERRORLEVEL%
)
echo Writer program built successfully.

echo.

echo ===========================================
echo Building the reader program...
echo ===========================================
gcc -o reader.exe reader.c -lwinmm
if %ERRORLEVEL% neq 0 (
    echo Failed to compile reader.c
    exit /b %ERRORLEVEL%
)
echo Reader program built successfully.

echo.

echo ===========================================
echo Starting the writer and reader programs...
echo ===========================================
start writer.exe
start reader.exe
echo Programs are running.

echo.

echo Press any key to close this window.
pause > nul
