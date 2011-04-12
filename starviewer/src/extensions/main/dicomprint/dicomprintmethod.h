#ifndef UDGDICOMPRINTMETHOD_H
#define UDGDICOMPRINTMETHOD_H

#include "printmethod.h"

/**
* Implementació per la Interfície PrintMethod per impressores Dicom en DCMTK.
*/

namespace udg
{
    class Printer;
    class PrintJob;
    class DicomPrinter;
    class DicomPrintJob;

class DicomPrintMethod: public PrintMethod
{
public:

	 DicomPrintMethod();
	~DicomPrintMethod();

    void setPrinter(Printer &_printer);
    void setPrintJob(PrintJob &_printJob);
    bool sentToPrint();

private:

    DicomPrinter * m_printer;
    DicomPrintJob * m_printJob;
};
}; 
#endif