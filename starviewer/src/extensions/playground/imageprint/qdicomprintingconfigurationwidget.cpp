#include <iostream>
#include "qdicomprintingconfigurationwidget.h"
#include "printer.h"
#include "dicomprinter.h"

namespace udg {

QDicomPrintingConfigurationWidget::QDicomPrintingConfigurationWidget()
{
    setupUi( this );
    createActions();	
    createConnections();
}

QDicomPrintingConfigurationWidget::~QDicomPrintingConfigurationWidget()
{

}

void QDicomPrintingConfigurationWidget::createConnections()
{ 
	
}

void QDicomPrintingConfigurationWidget::createActions()
{
	
}

void QDicomPrintingConfigurationWidget::setPrinter(Printer& _printer)
{

}

Printer* QDicomPrintingConfigurationWidget::getPrinter()
{
    Printer * prova=new DicomPrinter();
    return prova;
}
}                                      