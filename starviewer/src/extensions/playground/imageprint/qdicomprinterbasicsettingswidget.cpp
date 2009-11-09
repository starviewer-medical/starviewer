#include <QWidget>

#include "qdicomprinterbasicsettingswidget.h"

#include "dicomprinter.h"

namespace udg {

QDicomPrinterBasicSettingsWidget::QDicomPrinterBasicSettingsWidget( QWidget *parent )
{
    setupUi( this );
}

void QDicomPrinterBasicSettingsWidget::getDicomPrinterBasicSettings(DicomPrinter &dicomPrinter)
{
    getFilmSettings(dicomPrinter);
    getPrintSettings(dicomPrinter);
}

void QDicomPrinterBasicSettingsWidget::setDicomPrinterBasicSettings(DicomPrinter &dicomPrinter)
{
    setFilmSettings(dicomPrinter);
    setPrintSettings(dicomPrinter);
}

void QDicomPrinterBasicSettingsWidget::clear()
{
    // Print Settings
    m_priorityComboBox->clear();
    m_mediumTypeComboBox->clear();
    m_filmDestinationComboBox->clear();
    
    // Film Settings
    m_layoutComboBox->clear();
    m_filmOrientationComboBox->clear();        
    m_filmSizeComboBox->clear();
}

void QDicomPrinterBasicSettingsWidget::getFilmSettings(DicomPrinter& printer)
{
    printer.setDefaultFilmLayout(m_layoutComboBox->currentText());
    printer.setDefaultFilmOrientation(m_filmOrientationComboBox->currentText());
    printer.setDefaultFilmSize(m_filmSizeComboBox->currentText());
}

void QDicomPrinterBasicSettingsWidget::getPrintSettings(DicomPrinter& printer)
{
    printer.setDefaultPrintPriority(m_priorityComboBox->currentText());   
    printer.setDefaultMediumType(m_mediumTypeComboBox->currentText());
    printer.setDefaultFilmDestination(m_filmDestinationComboBox->currentText());
}

void QDicomPrinterBasicSettingsWidget::setFilmSettings(DicomPrinter& printer)
{
    m_layoutComboBox->addItems(printer.getAvailableFilmLayoutValues());
    m_layoutComboBox->setCurrentIndex(m_layoutComboBox->findText(printer.getDefaultFilmLayout()));    
    m_filmOrientationComboBox->addItems(printer.getAvailableFilmOrientationValues());
    m_filmOrientationComboBox->setCurrentIndex(m_filmOrientationComboBox->findText(printer.getDefaultFilmOrientation()));
    m_filmSizeComboBox->addItems(printer.getAvailableFilmSizeValues());
    m_filmSizeComboBox->setCurrentIndex(m_filmSizeComboBox->findText(printer.getDefaultFilmSize()));
}

void QDicomPrinterBasicSettingsWidget::setPrintSettings(DicomPrinter& printer)
{
    m_priorityComboBox->addItems(printer.getAvailablePrintPriorityValues());
    m_priorityComboBox->setCurrentIndex(m_priorityComboBox->findText(printer.getDefaultPrintPriority()));
    m_mediumTypeComboBox->addItems(printer.getAvailableMediumTypeValues());
    m_mediumTypeComboBox->setCurrentIndex(m_mediumTypeComboBox->findText(printer.getDefaultMediumType()));
    m_filmDestinationComboBox->addItems(printer.getAvailableFilmDestinationValues());
    m_filmDestinationComboBox->setCurrentIndex(m_filmDestinationComboBox->findText(printer.getDefaultFilmDestination()));
}

}
