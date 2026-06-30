@echo off
setlocal
cd /d "%~dp0"
python "python\engine_ui.py"
if errorlevel 1 (
    echo Failed to start the engine editor.
    pause
    exit /b 1
)
exit /b 0
