var ForReading = 1;
var ForWriting = 2;

var makefileName = WScript.Arguments(0);
var fileSystemObject = new ActiveXObject("Scripting.FileSystemObject");
var inputTextStream = fileSystemObject.OpenTextFile(makefileName, ForReading);
var makefile = inputTextStream.ReadAll();
inputTextStream.Close();

var regExp = /(c\:\\Qt\\[0-9]\.[0-9]+\\lib\\)([A-Za-z]+)d(4?)(\.lib)/gi;
var WshShell = WScript.CreateObject("WScript.Shell");

if (regExp.test(makefile)) {
    WScript.Echo("Fixing " + WshShell.CurrentDirectory + "\\" + makefileName + "...");
    var modifiedMakefile = makefile.replace(regExp, "$1$2$3$4");
    var outputTextStream = fileSystemObject.OpenTextFile(makefileName, ForWriting);
    outputTextStream.Write(modifiedMakefile);
    outputTextStream.Close();
}
else {
    WScript.Echo("No need to fix " + WshShell.CurrentDirectory + "\\" + makefileName);
}
