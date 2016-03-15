mkdir "%BuildDir%"
pushd "%BuildDir%"

if exist "%PatchesRoot%\%BuildLib%" (
    set PatchedSourceDir=%SourceDir%-patched
    robocopy %SourceDir% !PatchedSourceDir! * /S
    pushd "!PatchedSourceDir!"

    for %%p in ("%PatchesRoot%\%BuildLib%\*") do (
        %Git% apply %%p --ignore-whitespace -v
    )

    popd
    set SourceDir=!PatchedSourceDir!
)

%CMake% %CMakeOptions% "%SourceDir%"
nmake
nmake install

popd
