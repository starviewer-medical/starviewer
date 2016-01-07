mkdir "%BuildDir%"
pushd %BuildDir%

cl %SourceDir%\sqlite3.c -O2 -DSQLITE_ENABLE_RTREE -DSQLITE_ENABLE_COLUMN_METADATA -link -dll -def:%ScriptsRoot%\libs\sqlite3.def -out:%BuildDir%\sqlite3.dll

mkdir "%InstallDir%"

copy %SourceDir%\sqlite3.h  %InstallDir%
copy %BuildDir%\sqlite3.dll %InstallDir%
copy %BuildDir%\sqlite3.lib %InstallDir%

popd
