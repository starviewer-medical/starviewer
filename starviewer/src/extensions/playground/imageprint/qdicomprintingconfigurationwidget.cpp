#include "qdicomprintingconfigurationwidget.h"

#include <iostream> //TODO: és necessaria ?

#include "printer.h"
#include "dicomprinter.h"
#include "fakeobjects.h"
#include "dicomprintjob.h"
#include "dicomprintpage.h"
#include "../core/image.h"
#include "../core/logging.h"
#include "dicomprint.h"


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
    connect(m_testButton, SIGNAL(clicked()), SLOT(testPrinting()));	
}

void QDicomPrintingConfigurationWidget::createActions()
{

}

void QDicomPrintingConfigurationWidget::testPrinting()
{
    FakeObjects fakeObjects;
    DicomPrintJob dicomPrintJob;
    DicomPrint dicomPrint;
    DicomPrinter dicomPrinter;

    dicomPrinter = fakeObjects.getFakeDicomPrinter();
    dicomPrintJob = fakeObjects.getFakeDicomPrintJob("1.2.826.0.1.3680043.2.403.1.200.43.20090217212000.332603502", "1.3.12.2.1107.5.1.4.50736.30000009021706425592100008899", 8);

    dicomPrint.print(dicomPrinter, dicomPrintJob);
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