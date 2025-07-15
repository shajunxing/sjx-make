@echo off
cd /d %~dp0
md bin
set cc=cl /nologo /std:clatest /utf-8 /O2 /W3 /MD
%cc% /Fobin\sleep.obj /Febin\sleep.exe src\sleep.c && %CC% /Fobin\test.obj /Febin\test.exe src\test.c && bin\test.exe %*
