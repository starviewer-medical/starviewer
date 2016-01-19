setlocal 

set SDKDIR=D:\SDK-0.13-64bit

mkdir %SDKDIR%

pushd %SDKDIR%

REM Generate script file to easily install the SDK in case the destination folder is not empty
echo set InstallDir=%%SystemDrive%% > %SDKDIR%\install-sdk.cmd
echo set ScriptsRoot=%%CD%% >> %SDKDIR%\install-sdk.cmd
echo robocopy %%ScriptsRoot%% %%InstallDir%% /E /XF install-sdk.cmd >> %SDKDIR%\install-sdk.cmd

robocopy "%SystemDrive%\dcmtk\3.6.1_20120515-64" "dcmtk\3.6.1_20120515-64" /MIR
robocopy "%SystemDrive%\gdcm\2.4.4-64" "gdcm\2.4.4-64" /MIR
robocopy "%SystemDrive%\InsightToolkit\4.7.1-64" "InsightToolkit\4.7.1-64" /MIR
robocopy "%SystemDrive%\ThreadWeaver\5.3.0-64" "ThreadWeaver\5.3.0-64" /MIR
robocopy "%SystemDrive%\vtk\6.1.0-64" "vtk\6.1.0-64" /MIR

popd

endlocal
