#ifndef UDGDICOMPRINTERHANDLER_H
#define UDGDICOMPRINTERHANDLER_H

#include <QList>
#include "printerhandler.h"

/**
* Implementació per la Interfície PrinterHandler per impressores Dicom en DCMTK.
*/

namespace udg
{
    class Printer;

class DicomPrinterHandler: public PrinterHandler
{
 public:
	 DicomPrinterHandler();
	~DicomPrinterHandler();

    void addPrinter(Printer &_printer);
    void updatePrinter(Printer &_printer);
    void removePrinter(Printer &_printer);
    void removePrinter(QString &_reference);
    Printer * getPrinter(QString &_reference);
};
}; 
#endif