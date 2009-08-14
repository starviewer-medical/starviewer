#include <QString>
#include <QSettings>

#include "dicomprinter.h"
#include "dicomprintjob.h"
#include "dicomprintmethod.h"

namespace udg 
{	
DicomPrintMethod::DicomPrintMethod()
{	
}
 
DicomPrintMethod::~DicomPrintMethod()
{
}

void DicomPrintMethod::setPrinter(Printer &_printer)
{	
    //m_printer=(DicomPrinter)_printer;
}

void DicomPrintMethod::setPrintJob(PrintJob &_printJob)
{
    //m_printJob=(DicomPrintJob)_printJob;
}

bool DicomPrintMethod::sentToPrint()
{
    return true;
}
}