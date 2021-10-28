@ECHO OFF

SETLOCAL ENABLEDELAYEDEXPANSION

TITLE Cleaning libOpenCOR...

SET AppDir=%~dp0

FOR /D %%I IN (_skbuild build src\bindings\python\libopencor.egg-info) DO IF EXIST !AppDir!%%I RMDIR /S /Q !AppDir!%%I
