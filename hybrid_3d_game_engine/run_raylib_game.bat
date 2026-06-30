@echo off
setlocal
cd /d "%~dp0"
where python >nul 2>nul
if errorlevel 1 (
    echo Python was not found in PATH.
    pause
    exit /b 1
)
python "python\raylib_game.py"
if errorlevel 1 (
    echo Failed to start the Raylib game.
    pause
    exit /b 1
)
exit /b 0
