@ECHO OFF

SETLOCAL ENABLEDELAYEDEXPANSION

REM Current version from `VERSION.txt`.

SET VersionFile=%~dp0VERSION.txt

FOR /F "usebackq delims=" %%I IN ("%VersionFile%") DO SET OldVersion=%%I

FOR /F "tokens=1-3 delims=." %%A IN ("!OldVersion!") DO (
    SET OldMajorVersion=%%A
    SET OldMinorVersion=%%B
    SET OldPatchVersion=%%C
)

REM Determine the new version based on the current version and the current date.

FOR /F "tokens=2 delims==" %%I IN ('wmic os get localdatetime /value') DO SET Now=%%I

SET NewMajorVersion=!OldMajorVersion!
SET NewMinorVersion=!Now:~0,8!

IF !OldMinorVersion! LSS !NewMinorVersion! (
    SET NewPatchVersion=0
) ELSE (
    SET /A NewPatchVersion=!OldPatchVersion! + 1
)

SET NewVersion=!NewMajorVersion!.!NewMinorVersion!.!NewPatchVersion!

REM Update our version file.

ECHO !NewVersion! > "%VersionFile%"

REM Display the old and new versions.

ECHO Old version: !OldVersion!
ECHO New version: !NewVersion!
