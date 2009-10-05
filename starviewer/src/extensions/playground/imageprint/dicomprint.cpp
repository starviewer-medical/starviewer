#include "dicomprint.h"

#include <QDir>
#include <QStringList>

#include "dicomprinter.h"
#include "dicomprintjob.h"
#include "createdicomprintspool.h"
#include "printdicomspool.h"
#include "../core/settings.h"
#include "imageprintsettings.h"

namespace udg
{
void DicomPrint::print(DicomPrinter printer, DicomPrintJob printJob)
{
    CreateDicomPrintSpool dicomPrintSpool;
    PrintDicomSpool printDicomSpool;
    QString storedDcmtkFilePath;
    ImagePrintSettings imagePrintSettings;
    QStringList dcmtkStoredPrintPathFileList;

    imagePrintSettings.init();//TODO:A on s'ha de fer l'ini ? 

    //Per cada pàgina que tenim generem el fitxer storedPrint de dcmtk, cada fitxer és un FilmBox (una placa)
    foreach(DicomPrintPage dicomPrintPage, printJob.getDicomPrintPages())
    {
        dcmtkStoredPrintPathFileList.append(dicomPrintSpool.createPrintSpool(printer, dicomPrintPage, Settings().getValue(ImagePrintSettings::SpoolDirectory).toString()));
    }

    //Enviem a imprimir cada pàgina    
    foreach(QString dcmtkStoredPrintPathFile, dcmtkStoredPrintPathFileList)
    {
        printDicomSpool.printBasicGrayscale(printer, printJob, dcmtkStoredPrintPathFile, Settings().getValue(ImagePrintSettings::SpoolDirectory).toString());
    }
    //TODO: falta esborra el contingut del directori spool
}

}