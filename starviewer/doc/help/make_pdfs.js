/*
 Genera els documents d'ajuda en format PDF al directori /bin a punt per empaquetar amb l'instal·lador.
 Funciona només sota Windows.
 Per fer-ho fa servir el Word de l'Office. Ha de ser versió Office >= 2007.

 TODO: Falta tenirn en compte la versió Lite.
*/

var shell = WScript.CreateObject("WScript.Shell");
var saveToFolderName = shell.CurrentDirectory + "\\..\\..\\bin\\"

function docxToPdf(msword, sourceFileName)
{
    var fileSystem = WScript.CreateObject("Scripting.FileSystemObject");
    var saveToFileName = saveToFolderName + fileSystem.GetBaseName(sourceFileName) + ".pdf";
    if (fileSystem.FileExists(saveToFileName))
    {
        fileSystem.DeleteFile(saveToFileName);
    }
    // Obrim el document amb el word i el convertim a pdf
    msword.Documents.Open(sourceFileName);
    msword.ActiveDocument.SaveAs(saveToFileName, 17); // 17 magic number per wdFormatPDF
}

var shell = WScript.CreateObject("WScript.Shell");
var filenames = [shell.CurrentDirectory + "\\usermanual\\Starviewer_User_guide.docx",
                 shell.CurrentDirectory + "\\quickstartguide\\Starviewer_Quick_start_guide.docx",
                 shell.CurrentDirectory + "\\shortcuts\\Starviewer_Shortcuts_guide.docx"];
var msword = WScript.CreateObject("Word.Application");

for(var i = 0; i < filenames.length; ++i)
{
    docxToPdf(msword, filenames[i]);
}

msword.quit();

WScript.Echo("The help files have benn generated successfully on " + saveToFolderName);