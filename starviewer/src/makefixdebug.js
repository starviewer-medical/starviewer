var ForReading = 1;
var ForWriting = 2;

var WshShell = WScript.CreateObject("WScript.Shell");
var makefileName = WScript.Arguments(0);
var fileSystemObject = new ActiveXObject("Scripting.FileSystemObject");

if (fileSystemObject.FileExists(makefileName)) {
    var inputTextStream = fileSystemObject.OpenTextFile(makefileName, ForReading);
    var makefile = inputTextStream.ReadAll();
    inputTextStream.Close();

    var regExp = /(Qt)(5?)([A-Za-z]+)d(4?)(\.lib)/gi;

    if (regExp.test(makefile)) {
        WScript.Echo("Fixing " + WshShell.CurrentDirectory + "\\" + makefileName + "...");
        var modifiedMakefile = makefile.replace(regExp, "$1$2$3$4$5");
        var outputTextStream = fileSystemObject.OpenTextFile(makefileName, ForWriting);
        outputTextStream.Write(modifiedMakefile);
        outputTextStream.Close();
    }
    else {
        WScript.Echo("No need to fix " + WshShell.CurrentDirectory + "\\" + makefileName);
    }
}
else {
    WScript.Echo("Makefile not found: " + WshShell.CurrentDirectory + "\\" + makefileName);
}
