@echo off
cls

echo ===========================================
echo Building the writer program
echo ===========================================
gcc -o ipc_writer.exe ipc_writer.c -lwinmm
if %ERRORLEVEL% neq 0 (
    echo Failed to compile ipc_writer.c
    exit /b %ERRORLEVEL%
)
echo Writer program built successfully.
echo.
echo.

echo ===========================================
echo Building the reader program
echo ===========================================
gcc -o ipc_reader.exe ipc_reader.c -lwinmm
if %ERRORLEVEL% neq 0 (
    echo Failed to compile ipc_reader.c
    exit /b %ERRORLEVEL%
)
echo Reader program built successfully.
echo.
echo.

echo ===========================================
echo Starting the writer and reader programs
echo ===========================================
start ipc_writer.exe
start ipc_reader.exe
echo Programs are running . . . 
echo.
echo.

echo ===========================================
echo Meanwhile installing matplotlib
echo ===========================================
pip install matplotlib
echo ===========================================
echo.
echo.

echo After the ipc_writer.exe and ipc_reader.exe have completed their work PRESS any key to plot the graphs
pause > nul
echo.
echo.

echo ===========================================
echo Plotting graphs
echo ===========================================
python plotting_graphs/plot_state_changes.py
python plotting_graphs/plot_buffer_memory_usage.py
echo Finnished.
pause
