@echo off
setlocal enabledelayedexpansion

echo ============================================================
echo   Image Editor - Qt6 Build Script
echo ============================================================
echo.

:: ─── Try to find Qt6 automatically ───────────────────────────────────────────
set "QT_PREFIX="

:: Common Qt Online Installer paths
for %%P in (
    "C:\Qt\6.9.0\mingw_64"
    "C:\Qt\6.8.0\mingw_64"
    "C:\Qt\6.7.0\mingw_64"
    "C:\Qt\6.6.0\mingw_64"
    "C:\Qt\6.5.0\mingw_64"
    "I:\Qt\6.9.0\mingw_64"
    "I:\Qt\6.8.0\mingw_64"
    "I:\Qt\6.7.0\mingw_64"
    "%USERPROFILE%\Qt\6.9.0\mingw_64"
    "%USERPROFILE%\Qt\6.8.0\mingw_64"
    "%USERPROFILE%\Qt\6.7.0\mingw_64"
) do (
    if exist "%%~P\lib\cmake\Qt6\Qt6Config.cmake" (
        set "QT_PREFIX=%%~P"
        echo [OK] Found Qt6 at: %%~P
        goto :found_qt
    )
)

:not_found_qt
echo [ERROR] Qt6 was not found on this system.
echo.
echo  To install Qt6, please do ONE of the following:
echo.
echo  Option 1 - Qt Online Installer (recommended):
echo    1. Download from: https://www.qt.io/download-qt-installer-oss
echo    2. Run the installer
echo    3. Select: Qt 6.x ^> MinGW 64-bit
echo    4. Also select: Qt 6.x ^> Qt Svg (for icons)
echo    5. Run this script again
echo.
echo  Option 2 - Set QT_PREFIX manually:
echo    set "QT_PREFIX=C:\path\to\your\Qt\6.x.x\mingw_64"
echo    build.bat
echo.
pause
exit /b 1

:found_qt
:: ─── Find cmake ───────────────────────────────────────────────────────────────
where cmake >nul 2>&1
if %errorlevel% neq 0 (
    :: Try cmake from MinGW64
    if exist "I:\mingw64\bin\cmake.exe" (
        set "PATH=I:\mingw64\bin;%PATH%"
    ) else (
        echo [ERROR] cmake not found. Please add cmake to your PATH.
        pause
        exit /b 1
    )
)

:: ─── Find MinGW g++ ───────────────────────────────────────────────────────────
where g++ >nul 2>&1
if %errorlevel% neq 0 (
    if exist "I:\mingw64\bin\g++.exe" (
        set "PATH=I:\mingw64\bin;%PATH%"
        echo [OK] Using MinGW64 at I:\mingw64\bin
    ) else (
        echo [ERROR] g++ not found. Please add MinGW64 to your PATH.
        pause
        exit /b 1
    )
)

echo [OK] cmake found: 
cmake --version | findstr "cmake version"
echo [OK] g++ found:
g++ --version | findstr "g++"
echo.

:: ─── Configure and build ─────────────────────────────────────────────────────
set "BUILD_DIR=%~dp0build"

echo [1/3] Configuring with CMake...
cmake -S "%~dp0" -B "%BUILD_DIR%" ^
    -G "MinGW Makefiles" ^
    -DCMAKE_PREFIX_PATH="%QT_PREFIX%" ^
    -DCMAKE_BUILD_TYPE=Release ^
    -DCMAKE_C_COMPILER=gcc ^
    -DCMAKE_CXX_COMPILER=g++

if %errorlevel% neq 0 (
    echo [ERROR] CMake configuration failed.
    pause
    exit /b 1
)

echo.
echo [2/3] Building...
cmake --build "%BUILD_DIR%" --config Release -- -j4

if %errorlevel% neq 0 (
    echo [ERROR] Build failed.
    pause
    exit /b 1
)

echo.
echo [3/3] Creating images/ directory in build...
if not exist "%BUILD_DIR%\images" mkdir "%BUILD_DIR%\images"

:: Copy sample images to build directory
if exist "%~dp0images" (
    xcopy /E /I /Y "%~dp0images\*" "%BUILD_DIR%\images\" >nul 2>&1
)

echo.
echo ============================================================
echo   BUILD SUCCESSFUL!
echo   Executable: %BUILD_DIR%\ImageEditor.exe
echo ============================================================
echo.
echo Starting application...
start "" "%BUILD_DIR%\ImageEditor.exe"
pause
