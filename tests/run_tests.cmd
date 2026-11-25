@echo off
setlocal

set "SCRIPT_DIR=%~dp0"
for %%I in ("%SCRIPT_DIR%..") do set "ROOT_DIR=%%~fI"
if not defined BUILD_DIR set "BUILD_DIR=%ROOT_DIR%\build"

cmake -S "%ROOT_DIR%" -B "%BUILD_DIR%" -Dorm_tests=ON
if errorlevel 1 exit /b %errorlevel%

cmake --build "%BUILD_DIR%" --parallel
if errorlevel 1 exit /b %errorlevel%

ctest --test-dir "%BUILD_DIR%" --output-on-failure %*
exit /b %errorlevel%
