#include "dicomprintfactory.h"
#include "qdicomprinterconfigurationwidget.h"
#include "qdicomjobcreatorwidget.h"
#include "dicomprinter.h"
#include "dicomprintmethod.h"
#include "dicomprintermanager.h"

namespace udg {

DicomPrintFactory::DicomPrintFactory()
{
    m_printer = new DicomPrinter();
    m_printMethod = new DicomPrintMethod();
    m_printerManager = new DicomPrinterManager();
    m_qPrinterconfigurationWidget = new QDicomPrinterConfigurationWidget();
    m_qPrintJobCreatorWidget = new QDicomJobCreatorWidget();
}

DicomPrintFactory::~DicomPrintFactory()
{

}

Printer* DicomPrintFactory::getPrinter()
{	
    return m_printer;
}

PrintMethod* DicomPrintFactory::getPrintMethod()
{
    return m_printMethod;
}

PrinterManager* DicomPrintFactory::getPrinterManager()
{
    return m_printerManager;
}

QPrinterConfigurationWidget* DicomPrintFactory::getPrinterConfigurationWidget()
{
    return m_qPrinterconfigurationWidget;
}

QPrintJobCreatorWidget* DicomPrintFactory::getPrintJobCreatorWidget()
{
    return m_qPrintJobCreatorWidget;
}
}
