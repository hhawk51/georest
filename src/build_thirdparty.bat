@echo off
call set_env_variables
SET BUILDCONFIG=Release
SET PLATFORM=Win32
echo [build]: CppUnit (%BUILDCONFIG%, %PLATFORM%)
pushd %CPPUNIT_PATH%\src
msbuild /p:Configuration="%BUILDCONFIG% Static" /p:Platform=%PLATFORM% CppUnitLibraries.sln
if "%errorlevel%"=="1" goto error
popd
echo [build]: CTemplate (%BUILDCONFIG%, %PLATFORM%)
pushd %CTEMPLATE_PATH%
msbuild /p:Configuration=%BUILDCONFIG% /p:Platform=%PLATFORM% google-ctemplate.sln
if "%errorlevel%"=="1" goto error
popd
echo [build]: FastCGI (%BUILDCONFIG%, %PLATFORM%)
pushd %FASTCGI_PATH%\Win32
msbuild /p:Configuration=%BUILDCONFIG% /p:Platform=%PLATFORM% libfcgi.vcproj
if "%errorlevel%"=="1" goto error
popd
echo [build]: LibKML (%BUILDCONFIG%, %PLATFORM%)
pushd %LIBKML_PATH%\msvc
msbuild /p:Configuration=%BUILDCONFIG% /p:Platform=%PLATFORM% libkml.sln
if "%errorlevel%"=="1" goto error
popd
echo [build]: POCO (%BUILDCONFIG%, %PLATFORM%)
pushd %POCO_PATH%
call build_vs90.cmd
popd
echo [build]: ZLib (%BUILDCONFIG%, %PLATFORM%)
pushd %ZLIB_PATH%\contrib\vstudio\vc9
msbuild /p:Configuration=%BUILDCONFIG% /p:Platform=%PLATFORM% zlibvc.sln
if "%errorlevel%"=="1" goto error
popd
goto quit
:error
echo [ERROR]: There was an error building the component
exit /B 1
:quit