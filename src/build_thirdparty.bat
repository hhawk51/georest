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
goto copydlls
:error
echo [ERROR]: There was an error building the component
exit /B 1
:copydlls
echo [build]: Copying dlls
SET OUTDIR=%CD%\..\bin\%PLATFORM%\%BUILDCONFIG%
pushd %CTEMPLATE_PATH%\%BUILDCONFIG%
copy /Y libctemplate.dll "%OUTDIR%"
popd
pushd %FASTCGI_PATH%\libfcgi\%BUILDCONFIG%
copy /Y libfcgi.dll "%OUTDIR%"
popd
pushd %POCO_PATH%\bin
if "%BUILDCONFIG%" == "Release" (
copy /Y PocoFoundation.dll "%OUTDIR%"
copy /Y PocoNet.dll "%OUTDIR%"
copy /Y PocoUtil.dll "%OUTDIR%"
copy /Y PocoXML.dll "%OUTDIR%"
) else (
copy /Y PocoFoundationd.* "%OUTDIR%"
copy /Y PocoNetd.* "%OUTDIR%"
copy /Y PocoUtild.* "%OUTDIR%"
copy /Y PocoXMLd.* "%OUTDIR%"
)
popd
pushd %ZLIB_PATH%\contrib\vstudio\vc9\x86\ZlibDll%BUILDCONFIG%
copy /Y zlibwapi.dll "%OUTDIR%"
popd