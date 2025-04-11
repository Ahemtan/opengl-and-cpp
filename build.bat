@echo off
setlocal enabledelayedexpansion

rem === Config ===
set BUILD_DIR=build
set BUILD_TYPE=Debug
set EXE_NAME=virtuniverse.exe

rem === Optional: Clean Build ===
if /I "%CLEAN_BUILD%"=="true" (
    echo [INFO] Cleaning build directory...
    if exist %BUILD_DIR% rmdir /s /q %BUILD_DIR%
)

rem === Ensure Build Directory Exists ===
if not exist %BUILD_DIR% (
    echo [INFO] Creating build directory...
    mkdir %BUILD_DIR%
)

rem === Move into Build Directory ===
pushd %BUILD_DIR%

rem === Run Conan Install ===
echo [INFO] Installing Conan dependencies...
conan install .. --output-folder=. --build=missing --settings=build_type=%BUILD_TYPE%
if errorlevel 1 (
    echo [ERROR] Conan install failed.
    popd
    exit /b 1
)

rem === Run CMake Configure ===
echo [INFO] Configuring CMake...
cmake .. -DCMAKE_BUILD_TYPE=%BUILD_TYPE% -G "MinGW Makefiles" -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake
if errorlevel 1 (
    echo [ERROR] CMake configuration failed.
    popd
    exit /b 1
)

rem === Build the Project ===
echo [INFO] Building the project...
cmake --build .
if errorlevel 1 (
    echo [ERROR] Build failed.
    popd
    exit /b 1
)

rem === Run the Executable with Logging ===
echo [INFO] Running the application...
echo ---------------- Application Output ----------------
%EXE_NAME%
echo -----------------------------------------------------
echo [INFO] Application exited with code %ERRORLEVEL%

rem === Optional: Pause to Keep Console Open ===
pause

rem === Return to Original Directory ===
popd
endlocal
