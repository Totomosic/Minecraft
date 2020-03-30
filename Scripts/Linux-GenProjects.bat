@echo off
pushd %~dp0\..\
call Bolt\vendor\bin\Windows\premake\premake5.exe --os=linux gmake2
popd
PAUSE