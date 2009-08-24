#include "dicomprint.h"

#include "dicomprinter.h"
#include "dicomprintjob.h"
#include "createdicomprintspool.h"

namespace udg
{
void DicomPrint::print(DicomPrinter printer, DicomPrintJob printJob)
{
    CreateDicomPrintSpool dicomPrintSpool;

    dicomPrintSpool.createPrintSpool(printer, printJob);
}

}