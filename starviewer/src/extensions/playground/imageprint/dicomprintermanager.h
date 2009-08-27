#ifndef UDGDICOMPRINTERMANAGER_H
#define UDGDICOMPRINTERMANAGER_H

#include <QList>
#include "printermanager.h"

/**
* Implementació per la Interfície PrinterHandler per impressores Dicom en DCMTK.
*/

namespace udg
{
    class Printer;

class DicomPrinterManager: public PrinterManager
{
 public:
	 DicomPrinterManager();
	~DicomPrinterManager();

    void addPrinter(Printer &_printer);
    void updatePrinter(Printer &_printer);
    void removePrinter(Printer &_printer);
    void removePrinter(QString &_reference);
    Printer * getPrinter(QString &_reference);
};
}; 
#endif