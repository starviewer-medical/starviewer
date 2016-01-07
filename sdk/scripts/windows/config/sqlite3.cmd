set SourceDir=%SourceDirBase%\sqlite-amalgamation-3080803\sqlite-amalgamation-3080803

if %BuildType% == debug (
    set BuildDir=%SourceDir%-build-debug
    set InstallPrefix=%SystemDrive%\sqlite3d-%Arch%
)
if %BuildType% == release (
    set BuildDir=%SourceDir%-build-release
    set InstallDir=%SystemDrive%\sqlite3-%Arch%
)
