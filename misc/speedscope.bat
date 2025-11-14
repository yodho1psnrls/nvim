@echo off
REM --------------------------------------
REM https://github.com/jlfwong/speedscope
REM IF YOU INSTALL IT WITH NPM, YOU DONT NEED THIS BASH FILE
REM Install the speedscope standalone release, unzip it, create a bin/ folder where the index.html is located and place this scrip in that bin folder. Include the bin folder to PATH environmental variables
REM SpeedScope Launcher Usage: 
REM   - Without arguments: just open Speedscope index.html
REM   - With JSON file: visualize it
REM --------------------------------------
REM set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ftime-trace") # json file output (Clang)
REM set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ftime-report") # txt file output (GCC and Clang)
REM Generates json files which you can view in the browser
REM to profile what takes most time to compile for heavy template code
REM It is generated in Root\Release\src\CMakeFiles\proj.dir\main.cpp.json
REM Then in the browser go to either chrome://tracing or edge://tracing
REM Then from there on the top left press the Load button and find the file
REM --------------------------------------

REM SETLOCAL

REM Get folder of the bat file
SET "BIN_DIR=%~dp0"
REM Go one folder up to Speedscope root
SET "SCOPE_DIR=%BIN_DIR%..\"

IF "%~1"=="" (
    SETLOCAL EnableDelayedExpansion
    pushd "%~dp0.."
    SET "SCOPE_DIR=!CD!"
    popd
    SET "INDEX_FILE=!SCOPE_DIR!\index.html"
    REM echo "!SCOPE_DIR!"
    REM echo "!INDEX_FILE!"
    start "" "msedge" "--inprivate" "!INDEX_FILE!"
    ENDLOCAL
    exit /b 0
)

REM JSON file is provided
SET "JSON_FILE=%~1"
SET "LINK_FILE=%SCOPE_DIR%to_visualize.json"

REM Delete previous symlink if exists
IF EXIST "%LINK_FILE%" (
    DEL "%LINK_FILE%"
)

REM Create symbolic link /HARDLINK (doesnt require admin or Developer Mode)
mklink /H "%LINK_FILE%" "%JSON_FILE%"
IF ERRORLEVEL 1 (
    echo Failed to create symlink. Make sure Developer Mode is enabled.
    exit /b 1
)

REM Change directory to Speedscope folder
REM PUSHD "%SCOPE_DIR%"

REM Start Python HTTP server in background
REM Using python opens new console, Using pythonw runs it hidden
REM start "" python -m http.server 8000
REM start "" pythonw -m http.server 8000
REM start "" pythonw -m http.server 8000 --bind localhost
REM start "" python -m http.server 8000 --bind 127.0.0.1

REM You can check all processes on a specific port with "netstat -ano | findstr :8000"
REM Kill any existing python launched server on that port
REM for /f "tokens=5" %%a in ('netstat -ano ^| findstr :8080') do taskkill /PID %%a /F
REM Silent Version
for /f "tokens=5" %%a in ('netstat -ano ^| findstr :8080') do taskkill /PID %%a /F >nul 2>&1

REM echo Serving from "%~dp0\.."
start "" /B pythonw -m http.server 8080 --bind 127.0.0.1 --directory "%~dp0\.."


REM Wait a moment for the process to start
REM timeout /t 1 >nul
REM Get the most recent pythonw PID
REM for /f "tokens=2 delims=," %%p in ('tasklist /FI "IMAGENAME eq pythonw.exe" /FO CSV /NH') do (
REM     set "PY_PID=%%~p"
REM )
REM Strip quotes
REM set "PY_PID=%PY_PID:"=%"
REM echo [INFO] PID of pythonw server: %PY_PID%


REM Give the server a moment to start, before launching the browser
REM timeout /t 2 /nobreak >nul

REM Launch Edge in InPrivate with the profile
REM start "" "msedge" "--inprivate" "http://localhost:8000/index.html#profileURL=http://localhost:8000/to_visualize.json"
start "" "msedge" "--inprivate" "http://127.0.0.1:8080/index.html#profileURL=http://127.0.0.1:8080/to_visualize.json"


REM Optional: wait few seconds and then kill server
REM timeout /t 2 >nul
REM if defined PY_PID taskkill /PID %PY_PID% /F >nul 2>&1

REM POPD
REM ENDLOCAL
