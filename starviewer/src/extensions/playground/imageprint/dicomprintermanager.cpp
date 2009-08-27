#include <QString>
#include <QSettings>

#include "printer.h"
#include "printermanager.h"
#include "dicomprintermanager.h"
#include "dicomprinter.h"

namespace udg 
{	
DicomPrinterManager::DicomPrinterManager()
{	
}

 
DicomPrinterManager::~DicomPrinterManager()
{
}

void DicomPrinterManager::addPrinter(Printer &_printer)
{	

}

void DicomPrinterManager::updatePrinter(Printer &_printer)
{

}

void DicomPrinterManager::removePrinter(Printer &_printer)
{

}

void DicomPrinterManager::removePrinter(QString &_reference)
{ 
	
}

Printer * DicomPrinterManager::getPrinter(QString &_reference)
{
    Printer * prova=new DicomPrinter();
    return prova;
}
}