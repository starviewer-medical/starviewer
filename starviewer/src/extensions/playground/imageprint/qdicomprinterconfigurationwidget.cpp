#include <iostream>
#include "qdicomprinterconfigurationwidget.h"
#include "printer.h"

namespace udg {

QDicomPrinterConfigurationWidget::QDicomPrinterConfigurationWidget()
{
    setupUi( this );
    createActions();	
    createConnections();
	createTranslation();
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

void QDicomPrinterConfigurationWidget::createTranslation()
{
    m_qgbPrinter->setTitle(tr("Printer"));
    m_qlAetitle->setText(tr("AETitle"));
    m_qlDescription->setText(tr("Description"));
    m_qlHostname->setText(tr("Hostname"));
    m_qlPort->setText(tr("Port"));

    m_qgbDefaultPrinterSettings->setTitle(tr("Default Printer Settings"));
    m_qlOrientation->setText(tr("Film Orientation"));
    m_qlLayout->setText(tr("Layout"));
    m_qlSize->setText(tr("Film Size"));
    m_qlMaximDensity->setText(tr("Max. Density"));
    m_qlMiniumDensity->setText(tr("Min. Density"));
    m_qlDestination->setText(tr("Destination"));
    m_qlEmptyDensity->setText(tr("Empty Density"));
    m_qlSmoothing->setText(tr("Smoothing"));
    m_qlMagnifaction->setText(tr("Magnifaction"));
    m_qlBorderDensity->setText(tr("Border Density"));
    m_qchbVisibleTrim->setText(tr("Visible Trim"));

    m_qgbDefaultJobSettings->setTitle(tr("Default Job Settings"));
    m_qllMedium->setText(tr("Medium"));
    m_qlPriority->setText(tr("Priority"));
    m_qcblCopies->setText(tr("Number of Copies"));

    m_qpbAdd->setText(tr("Add"));
    m_qpbDelete->setText(tr("Delete"));
    m_qpbUpdate->setText(tr("Update"));
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