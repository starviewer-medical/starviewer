call "%ScriptsRoot%\libs\cmakebuild.cmd"

if %BuildType% == debug (
    copy %ZlibDir%\lib\zlib.lib %ZlibDir%\lib\zlib_d.lib 
)
if %BuildType% == release (
    copy %ZlibDir%\lib\zlib.lib %ZlibDir%\lib\zlib_o.lib 
)
