#include "dicomprintfactory.h"
#include "qdicomprinterconfigurationwidget.h"
#include "qdicomprintingconfigurationwidget.h"
#include "qdicomjobcreatorwidget.h"
#include "dicomprinter.h"
#include "dicomprintmethod.h"
#include "dicomprinterhandler.h"

namespace udg {

DicomPrintFactory::DicomPrintFactory()
{
    m_printer                       =new DicomPrinter();
    m_printMethod                   =new DicomPrintMethod();
    m_printerHandler                =new DicomPrinterHandler();
    m_qPrinterconfigurationWidget   =new QDicomPrinterConfigurationWidget();
    m_qPrintingConfigurationWidget  =new QDicomPrintingConfigurationWidget();
    m_qPrintJobCreatorWidget        =new QDicomJobCreatorWidget();
}

DicomPrintFactory::~DicomPrintFactory()
{
}

Printer * DicomPrintFactory::getPrinter()
{	
	return m_printer;
}

PrintMethod	* DicomPrintFactory::getPrintMethod()
{
    return m_printMethod;
}

PrinterHandler	* DicomPrintFactory::getPrinterHandler()
{
    return m_printerHandler;
}

QPrinterConfigurationWidget * DicomPrintFactory::getPrinterConfigurationWidget()
{
	return m_qPrinterconfigurationWidget;
}

QPrintingConfigurationWidget* DicomPrintFactory::getPrintingConfigurationWidget()
{
	return m_qPrintingConfigurationWidget;
}

QPrintJobCreatorWidget	* DicomPrintFactory::getPrintJobCreatorWidget()
{
    return m_qPrintJobCreatorWidget;
}
}