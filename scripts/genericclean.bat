@ECHO OFF

SETLOCAL ENABLEDELAYEDEXPANSION

IF "%1" == "all" (
    TITLE Cleaning all of libOpenCOR...
) ELSE (
    TITLE Cleaning libOpenCOR...
)

SET AppDir=%~dp0..\
SET OrigDir=!CD!

FOR /D %%I IN (_skbuild build src\bindings\python\libopencor.egg-info) DO IF EXIST !AppDir!%%I RMDIR /S /Q !AppDir!%%I

IF "%1" == "all" (
    IF EXIST !AppDir!prebuilt (
        CD !AppDir!prebuilt

        FOR /D %%I IN (*.*) DO RMDIR /S /Q %%I
        FOR    %%I IN (*.*) DO IF NOT %%I == ".gitignore" DEL /Q %%I
    )
)

CD !OrigDir!
