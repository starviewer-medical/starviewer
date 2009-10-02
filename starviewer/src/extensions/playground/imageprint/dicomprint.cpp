#include "dicomprint.h"

#include <QDir>

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

    imagePrintSettings.init();//TODO:A on s'ha de fer l'ini ? 

    storedDcmtkFilePath = dicomPrintSpool.createPrintSpool(printer, printJob.getPrintPage(), Settings().getValue(ImagePrintSettings::SpoolDirectory).toString());

    printDicomSpool.printBasicGrayscale(printer, printJob, storedDcmtkFilePath, Settings().getValue(ImagePrintSettings::SpoolDirectory).toString());

    //TODO: falta esborra el contingut del directori spool
}

}