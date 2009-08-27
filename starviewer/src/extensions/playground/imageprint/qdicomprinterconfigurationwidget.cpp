#include <iostream>
#include "qdicomprinterconfigurationwidget.h"
#include "printer.h"
#include "dicomprinter.h";
#include "dicomprintermanager.h";

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

//TODO: S'hauria d'implementar un Singleton per accedir a DicomPrinterManager. 
// Falta corregir errors de tipus.

void  QDicomPrinterConfigurationWidget::choosePrinter(const int &id)
{
    DicomPrinterManager dicomPrinterManager;
    DicomPrinter selectedPrinter = dicomPrinterManager.getPrinterByID(id);
    
    // Printer
    m_printerAetitleLineEdit->setText(selectedPrinter.getAETitle());
    m_printerDescriptionLineEdit->setText(selectedPrinter.getDescription());
    m_printerHostnameLineEdit->setText(selectedPrinter.getHostname());
    //m_printerPortLineEdit->setText(selectedPrinter.getPort());

    // Default Job Settings
    m_priorityComboBox->addItems(selectedPrinter.getAvailablePrintPriorityValues());
    m_priorityComboBox->setCurrentIndex(m_priorityComboBox->findText(selectedPrinter.getDefaultPrintPriority()));
    m_mediumTypeComboBox->addItems(selectedPrinter.getAvailableMediumTypeValues());
    m_mediumTypeComboBox->setCurrentIndex(m_mediumTypeComboBox->findText(selectedPrinter.getDefaultMediumType()));

    // Default Printer Settings
    m_layoutComboBox->addItems(selectedPrinter.getAvailableFilmLayoutValues());
    m_layoutComboBox->setCurrentIndex(m_layoutComboBox->findText(selectedPrinter.getDefaultFilmLayout()));    
    m_filmOrientationComboBox->addItems(selectedPrinter.getAvailableFilmOrientationValues());
    m_filmOrientationComboBox->setCurrentIndex(m_filmOrientationComboBox->findText(selectedPrinter.getDefaultFilmOrientation()));
    m_filmSizeComboBox->addItems(selectedPrinter.getAvailableFilmSizeValues());
    m_filmSizeComboBox->setCurrentIndex(m_filmSizeComboBox->findText(selectedPrinter.getDefaultFilmSize()));
    m_filmDestinationComboBox->addItems(selectedPrinter.getAvailableFilmDestinationValues());
    m_filmDestinationComboBox->setCurrentIndex(m_filmDestinationComboBox->findText(selectedPrinter.getDefaultFilmDestination()));
    m_magnifactionTypeComboBox->addItems(selectedPrinter.getAvailableMagnificationTypeValues());
    m_magnifactionTypeComboBox->setCurrentIndex(m_magnifactionTypeComboBox->findText(selectedPrinter.getDefaultMagnificationType()));
    m_smoothingTypeComboBox->addItems(selectedPrinter.getAvailableSmoothingTypeValues());
    m_smoothingTypeComboBox->setCurrentIndex(m_smoothingTypeComboBox->findText(selectedPrinter.getDefaultSmoothingType()));
    m_borderDensityComboBox->addItems(selectedPrinter.getAvailableBorderDensityValues());
    m_borderDensityComboBox->setCurrentIndex(m_borderDensityComboBox->findText(selectedPrinter.getDefaultBorderDensity()));
    m_emptyDensityComboBox->addItems(selectedPrinter.getAvailableEmptyImageDensityValues());
    m_emptyDensityComboBox->setCurrentIndex(m_emptyDensityComboBox->findText(selectedPrinter.getDefaultEmptyImageDensity()));
    /*
    m_maximDensitySpinBox->setMaximum(selectedPrinter.getAvailableMaxDensityValues);
    m_maximDensitySpinBox->setValue(selectedPrinter.getDefaultMaxDensity);
    m_miniumDensitySpinBox->setMaximum(selectedPrinter.getAvailableMinDensityValues);
    m_miniumDensitySpinBox->setValue(selectedPrinter.getDefaultMinDensity);
*/
    if(selectedPrinter.getAvailableTrim())
    {
        m_visibleTrimCheckBox->setCheckable(true);
        m_visibleTrimCheckBox->setChecked(selectedPrinter.getDefaultTrim());
    }
    else
    {   
        m_visibleTrimCheckBox->setChecked(false);
        m_visibleTrimCheckBox->setCheckable(false);
    }
}
}                                      