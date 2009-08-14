#include <QString>
#include <QSettings>

#include "printer.h"
#include "printerhandler.h"
#include "dicomprinterhandler.h"
#include "dicomprinter.h"

namespace udg 
{	
DicomPrinterHandler::DicomPrinterHandler()
{	
}

 
DicomPrinterHandler::~DicomPrinterHandler()
{
}

void DicomPrinterHandler::addPrinter(Printer &_printer)
{	

}

void DicomPrinterHandler::updatePrinter(Printer &_printer)
{

}

void DicomPrinterHandler::removePrinter(Printer &_printer)
{

}

void DicomPrinterHandler::removePrinter(QString &_reference)
{ 
	
}

Printer * DicomPrinterHandler::getPrinter(QString &_reference)
{
    Printer * prova=new DicomPrinter();
    return prova;
}
}