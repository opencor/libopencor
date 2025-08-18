@ECHO OFF

SETLOCAL ENABLEDELAYEDEXPANSION

REM Major version.

SET MajorVersion=0

REM Minor version.

FOR /F "tokens=2 delims==" %%I IN ('wmic os get localdatetime /value') DO SET Now=%%I
SET MinorVersion=!Now:~0,8!
ECHO "MinorVersion: !MinorVersion!"

REM Patch version.

SET PatchVersion=0

FOR /F %%I IN ('git rev-list --tags --max-count=1 2^> NUL') DO (
    SET LatestTagCommit=%%I
)
ECHO "LatestTagCommit: !LatestTagCommit!"

IF DEFINED LatestTagCommit (
    FOR /F %%I IN ('git describe --tags !LatestTagCommit! 2^> NUL') DO (
      SET GitDescribe=%%I
    )
    ECHO "GitDescribe: !GitDescribe!"

    FOR /F "tokens=2,3 delims=." %%A IN ("!GitDescribe!") DO (
        SET MinorVersion=%%A
        ECHO "MinorVersion: !MinorVersion!"
        SET PatchVersion=%%B
        ECHO "PatchVersion: !PatchVersion!"

        IF "!MinorVersion!"=="!MinorVersion!" (
            SET /A PatchVersion+=1
        ) ELSE (
            SET PatchVersion=0
        )
    )
)

REM Full version.

SET Version=!MajorVersion!.!MinorVersion!.!PatchVersion!

REM Update our version file.

ECHO !Version! > "%~dp0VERSION.txt"
