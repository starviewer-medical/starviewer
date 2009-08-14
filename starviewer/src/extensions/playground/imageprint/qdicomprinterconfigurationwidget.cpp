#include <iostream>
#include "qdicomprinterconfigurationwidget.h"
#include "printer.h"

namespace udg {

QDicomPrinterConfigurationWidget::QDicomPrinterConfigurationWidget()
{
    setupUi( this );
    createActions();	
    createConnections();
}

QDicomPrinterConfigurationWidget::~QDicomPrinterConfigurationWidget()
{

}

void QDicomPrinterConfigurationWidget::createConnections()
{ 
	
}

void QDicomPrinterConfigurationWidget::createActions()
{
	
}

void QDicomPrinterConfigurationWidget::addPrinter(Printer &_printer)
{

}
	
void QDicomPrinterConfigurationWidget::modifyPrinter(Printer &_printer)
{

}

void QDicomPrinterConfigurationWidget::deletePrinter(char * _refernce)
{

}
}                                      