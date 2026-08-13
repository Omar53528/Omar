@echo off
setlocal enabledelayedexpansion

echo ==========================================
echo    Omega Engine - Build Script (UCRT64)
echo ==========================================
echo.

REM Check if we are in UCRT64 environment
if not defined MSYSTEM (
    echo [ERROR] Please run this script from MSYS2 UCRT64 shell (ucrt64.exe).
    echo         Do not run from standard CMD or PowerShell.
    exit /b 1
)

if not "%MSYSTEM%" == "UCRT64" (
    echo [WARNING] You are running in %MSYSTEM%. Recommended is UCRT64.
    echo         Proceeding anyway...
)

REM Create build directory if not exists
if not exist "build" (
    echo [INFO] Creating build directory...
    mkdir build
)

REM Clean previous build to avoid conflicts
echo [INFO] Cleaning previous build artifacts...
if exist "build\CMakeCache.txt" del "build\CMakeCache.txt"
if exist "build\Makefile" del "build\Makefile"

REM Run CMake
echo [INFO] Configuring project with CMake...
cd build
cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release ..

if %ERRORLEVEL% neq 0 (
    echo.
    echo [ERROR] CMake configuration failed!
    echo        Check the errors above. Missing files might be the cause.
    cd ..
    exit /b 1
)

REM Build the project
echo.
echo [INFO] Building Omega Engine (using all available cores)...
cmake --build . --config Release -j4

if %ERRORLEVEL% neq 0 (
    echo.
    echo [ERROR] Build failed!
    cd ..
    exit /b 1
)

cd ..

echo.
echo ==========================================
echo    BUILD SUCCESSFUL!
echo    Executables located in: build/
echo    - OmegaEditor.exe
echo    - OMCompiler.exe
echo    - DemoGame.exe
echo ==========================================
pause
