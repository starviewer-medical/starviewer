#include "dicomprint.h"

#include <QDir>

#include "dicomprinter.h"
#include "dicomprintjob.h"
#include "createdicomprintspool.h"
#include "printdicomspool.h"
#include "../core/starviewerapplication.h"


namespace udg
{
void DicomPrint::print(DicomPrinter printer, DicomPrintJob printJob)
{
    CreateDicomPrintSpool dicomPrintSpool;
    PrintDicomSpool printDicomSpool;

    dicomPrintSpool.createPrintSpool(printer, printJob);
    //TODO:El càlcul del path del fitxer storedPrintDcm no ha de ser hardcoded
    printDicomSpool.printSpool(printer, printJob, UserDataRootPath + QDir::separator() + "Spool" + QDir::separator() + "storedPrint.dcm");
}

}