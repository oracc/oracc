estcmd wicked casket 1000
if errorlevel 1 goto error
estcmd wicked casket 1000
if errorlevel 1 goto error
estcmd regression casket
if errorlevel 1 goto error
rd /S /Q casket

estcmd gather casket .
if errorlevel 1 goto error
estcmd extkeys casket
if errorlevel 1 goto error
set QUERY_STRING=gnu
estseek.cgi
if errorlevel 1 goto error
rd /S /Q casket

@echo off
echo #================================
echo # SUCCESS
echo #================================
goto :EOF

:error
@echo off
echo #================================
echo # ERROR
echo #================================
goto :EOF
