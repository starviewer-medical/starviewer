#include <iostream>
#include "qdicomprinterconfigurationwidget.h"
#include "printer.h"
#include "dicomprinter.h"
#include "dicomprintermanager.h"
#include "starviewerapplication.h"
#include <QMessageBox>

namespace udg {

QDicomPrinterConfigurationWidget::QDicomPrinterConfigurationWidget()
{
    setupUi( this );
    createConnections();

    m_listPrintersTreeWidget->setColumnHidden(0,true); 
    clearPrinterSettings();
    refreshPrinterList();
    m_semafor=false;
}

QDicomPrinterConfigurationWidget::~QDicomPrinterConfigurationWidget()
{

}


//*** Public Slot***//

void  QDicomPrinterConfigurationWidget::printerSelectionChanged()
{
    m_semafor=true;
    DicomPrinterManager dicomPrinterManager;
    QTreeWidgetItem *selectedItem = 0;
    
    this->clearPrinterSettings();

    if( !m_listPrintersTreeWidget->selectedItems().isEmpty() )
    {   
        selectedItem = m_listPrintersTreeWidget->selectedItems().first();
        m_selectedPrinterId = selectedItem->text(0).toInt();
        
        DicomPrinter selectedPrinter = dicomPrinterManager.getPrinterByID(m_selectedPrinterId);
    
        this->setPrinterSettingsToControls(selectedPrinter);
        this->setDefaultPrinterSettingsToControls(selectedPrinter);
        this->setDefaultJobSettingsToControls(selectedPrinter);
    }
    m_semafor=false;
}

void QDicomPrinterConfigurationWidget::addPrinter()
{
    DicomPrinter dicomPrinter;
    DicomPrinterManager dicomPrinterManager;
    if (validatePrinterSettings())
    {   
        this->getPrinterSettingsFromControls(dicomPrinter);
        this->getDefaultPrinterSettingsFromControls(dicomPrinter);
        this->getDefaultJobSettingsFromControls(dicomPrinter);

        if ( !dicomPrinterManager.addPrinter(dicomPrinter) )
        {
            QMessageBox::warning(this, ApplicationNameString, tr("This Printer already exists."));
        }
        else
        {
            refreshPrinterList();
            clearPrinterSettings();
        }
    }
}

void QDicomPrinterConfigurationWidget::updatePrinter()
{
    DicomPrinter dicomPrinter;
    DicomPrinterManager dicomPrinterManager;
    if (validatePrinterSettings())
    {   
        this->getPrinterSettingsFromControls(dicomPrinter);
        this->getDefaultPrinterSettingsFromControls(dicomPrinter);
        this->getDefaultJobSettingsFromControls(dicomPrinter);

        if (!dicomPrinterManager.updatePrinter(m_selectedPrinterId,dicomPrinter))
        {
            QMessageBox::warning(this, ApplicationNameString, tr("This Printer already exists."));
        }
        else
        {
            refreshPrinterList();
            clearPrinterSettings();
        }
    }
}

void QDicomPrinterConfigurationWidget::deletePrinter()
{
    DicomPrinterManager dicomPrinterManager;
    dicomPrinterManager.deletePrinter(m_selectedPrinterId);
    this->refreshPrinterList();
}

void QDicomPrinterConfigurationWidget::testPrinter()
{

}

void  QDicomPrinterConfigurationWidget::getAvailableParameters()
{
    if(m_semafor) 
    {
        return;
    }

    QString text = m_printerAetitleLineEdit->text();
    if ( text.length() == 0 )
    {
        return;        
    }
    
    text = m_printerPortLineEdit->text();
    if ( !( text.toInt( NULL , 10 ) >= 0 && text.toInt( NULL , 10 ) <= 65535 ) || text.length() ==0 )
    {
       return;
    }

    DicomPrinterManager dicomPrinterManager;
    DicomPrinter printer = dicomPrinterManager.getAvailableParametersValues(m_printerAetitleLineEdit->text(),(m_printerPortLineEdit->text().toInt()));
    
    // Default Job Settings
    m_priorityComboBox->clear();
    m_priorityComboBox->addItems(printer.getAvailablePrintPriorityValues());        
    m_mediumTypeComboBox->clear();        
    m_mediumTypeComboBox->addItems(printer.getAvailableMediumTypeValues());
    m_mediumTypeComboBox->setCurrentIndex(-1);

    // Default Printer Settings
    m_layoutComboBox->clear();
    m_layoutComboBox->addItems(printer.getAvailableFilmLayoutValues());
    m_layoutComboBox->setCurrentIndex(-1);    
    m_filmOrientationComboBox->clear();
    m_filmOrientationComboBox->addItems(printer.getAvailableFilmOrientationValues());
    m_filmOrientationComboBox->setCurrentIndex(-1);    
    m_filmSizeComboBox->clear();
    m_filmSizeComboBox->addItems(printer.getAvailableFilmSizeValues());
    m_filmSizeComboBox->setCurrentIndex(-1);    
    m_filmDestinationComboBox->clear();
    m_filmDestinationComboBox->addItems(printer.getAvailableFilmDestinationValues());
    m_filmDestinationComboBox->setCurrentIndex(-1);    
    m_magnifactionTypeComboBox->clear();
    m_magnifactionTypeComboBox->addItems(printer.getAvailableMagnificationTypeValues());
    m_magnifactionTypeComboBox->setCurrentIndex(-1);    
    m_smoothingTypeComboBox->clear();
    m_smoothingTypeComboBox->addItems(printer.getAvailableSmoothingTypeValues());
    m_smoothingTypeComboBox->setCurrentIndex(-1);    
    m_borderDensityComboBox->clear();
    m_borderDensityComboBox->addItems(printer.getAvailableBorderDensityValues());
    m_borderDensityComboBox->setCurrentIndex(-1);    
    m_emptyDensityComboBox->clear();    
    m_emptyDensityComboBox->addItems(printer.getAvailableEmptyImageDensityValues());
    m_emptyDensityComboBox->setCurrentIndex(-1);    
    m_maximDensitySpinBox->setMaximum(printer.getAvailableMaxDensityValues());    
    m_maximDensitySpinBox->setValue(0);
    m_miniumDensitySpinBox->setMaximum(printer.getAvailableMinDensityValues());
    m_miniumDensitySpinBox->setValue(0);
    m_priorityComboBox->addItems(printer.getAvailablePrintPriorityValues());
    m_priorityComboBox->setCurrentIndex(-1);

    if(printer.getAvailableTrim())
    {
        m_visibleTrimCheckBox->setCheckable(true);
        m_visibleTrimCheckBox->setChecked(printer.getDefaultTrim());
    }
    else
    {   
        m_visibleTrimCheckBox->setChecked(false);
        m_visibleTrimCheckBox->setCheckable(false);
    }
}

//*** Private Methods***//

void QDicomPrinterConfigurationWidget::createConnections()
{ 
    connect( m_listPrintersTreeWidget , SIGNAL( itemSelectionChanged() ), SLOT( printerSelectionChanged() ) );

    connect( m_addPrinterPushButton , SIGNAL( clicked() ), SLOT( addPrinter() ));
    connect( m_updatePrinterPushButton , SIGNAL( clicked() ), SLOT( updatePrinter() ));
    connect( m_deletePrinterPushButton , SIGNAL( clicked() ), SLOT( deletePrinter() ));
    connect( m_verifyPrinterPushButton , SIGNAL( clicked() ), SLOT( testPrinter() ));    

    connect( m_printerAetitleLineEdit, SIGNAL( textChanged(const QString &) ), SLOT( getAvailableParameters() ) );
    connect( m_printerPortLineEdit, SIGNAL( textChanged(const QString &) ), SLOT( getAvailableParameters() ) );
}

void QDicomPrinterConfigurationWidget::refreshPrinterList()
{   
    DicomPrinterManager dicomPrinterManager;
    QList<DicomPrinter> dicomPrintersList = dicomPrinterManager.getDicomPrinterList();

    m_listPrintersTreeWidget->clear();

    foreach(DicomPrinter dicomPrinter, dicomPrintersList)
    {
        QTreeWidgetItem* item = new QTreeWidgetItem( m_listPrintersTreeWidget );

        item->setText(0, QString::number( m_listPrintersTreeWidget->topLevelItemCount()-1));
        item->setText(1, dicomPrinter.getAETitle());
        item->setText(2, dicomPrinter.getHostname());
        item->setText(3, QString::number( dicomPrinter.getPort()));
        item->setText(4, dicomPrinter.getDescription());
    }
}

bool QDicomPrinterConfigurationWidget::validatePrinterSettings()
{
    QString text;
    
    text = m_printerAetitleLineEdit->text();
    if ( text.length() == 0 )
    {
        QMessageBox::warning( this , ApplicationNameString , tr("AETitle field can't be empty") );
        return false;
    }
    
    text = m_printerHostnameLineEdit->text();
    if ( text.length() == 0 )
    {
        QMessageBox::warning( this , ApplicationNameString , tr ( "Incorrect server address" ) );
        return false;
    }

    //el port ha d'estar entre 0 i 65535
    text = m_printerPortLineEdit->text();
    if ( !( text.toInt( NULL , 10 ) >= 0 && text.toInt( NULL , 10 ) <= 65535 ) || text.length() ==0 )
    {
        QMessageBox::warning( this , ApplicationNameString , tr( "Printer Port has to be between 0 and 65535" ) );
        return false;
    }
    return true;
}

void QDicomPrinterConfigurationWidget::clearPrinterSettings()
{
    // Printer
    m_printerAetitleLineEdit->setText("");
    m_printerDescriptionLineEdit->setText("");
    m_printerHostnameLineEdit->setText("");
    m_printerPortLineEdit->setText("");

    // Default Job Settings
    m_numberCopiesSpinBox->setValue(0);
    m_priorityComboBox->setCurrentIndex(-1);
    m_mediumTypeComboBox->setCurrentIndex(-1);
    m_maximDensitySpinBox->setValue(0);
    m_maximDensitySpinBox->setValue(0);
    m_visibleTrimCheckBox->setChecked(false);

     // Default Job Settings
    m_priorityComboBox->clear();
    m_mediumTypeComboBox->clear();        
    m_mediumTypeComboBox->setCurrentIndex(-1);

    // Default Printer Settings
    m_layoutComboBox->clear();
    m_filmOrientationComboBox->clear();
    m_filmSizeComboBox->clear();
    m_filmDestinationComboBox->clear();
    m_magnifactionTypeComboBox->clear();
    m_smoothingTypeComboBox->clear();
    m_borderDensityComboBox->clear();    
    m_emptyDensityComboBox->clear();    
    m_maximDensitySpinBox->setValue(0);
    m_miniumDensitySpinBox->setValue(0);
}

void QDicomPrinterConfigurationWidget::setPrinterSettingsToControls(DicomPrinter& printer)
{
    m_printerAetitleLineEdit->setText(printer.getAETitle());
    m_printerDescriptionLineEdit->setText(printer.getDescription());
    m_printerHostnameLineEdit->setText(printer.getHostname());
    m_printerPortLineEdit->setText(QString::number( printer.getPort()));
}

void QDicomPrinterConfigurationWidget::getPrinterSettingsFromControls(DicomPrinter& printer)
{
    printer.setAETitle(m_printerAetitleLineEdit->text());
    printer.setDescription(m_printerDescriptionLineEdit->text());
    printer.setHostname(m_printerHostnameLineEdit->text());
    printer.setPort(m_printerPortLineEdit->text().toInt());
}

void QDicomPrinterConfigurationWidget::setDefaultPrinterSettingsToControls(DicomPrinter& printer)
{
    m_layoutComboBox->addItems(printer.getAvailableFilmLayoutValues());
    m_layoutComboBox->setCurrentIndex(m_layoutComboBox->findText(printer.getDefaultFilmLayout()));    
    m_filmOrientationComboBox->addItems(printer.getAvailableFilmOrientationValues());
    m_filmOrientationComboBox->setCurrentIndex(m_filmOrientationComboBox->findText(printer.getDefaultFilmOrientation()));
    m_filmSizeComboBox->addItems(printer.getAvailableFilmSizeValues());
    m_filmSizeComboBox->setCurrentIndex(m_filmSizeComboBox->findText(printer.getDefaultFilmSize()));
    m_filmDestinationComboBox->addItems(printer.getAvailableFilmDestinationValues());
    m_filmDestinationComboBox->setCurrentIndex(m_filmDestinationComboBox->findText(printer.getDefaultFilmDestination()));
    m_magnifactionTypeComboBox->addItems(printer.getAvailableMagnificationTypeValues());
    m_magnifactionTypeComboBox->setCurrentIndex(m_magnifactionTypeComboBox->findText(printer.getDefaultMagnificationType()));
    m_smoothingTypeComboBox->addItems(printer.getAvailableSmoothingTypeValues());
    m_smoothingTypeComboBox->setCurrentIndex(m_smoothingTypeComboBox->findText(printer.getDefaultSmoothingType()));
    m_borderDensityComboBox->addItems(printer.getAvailableBorderDensityValues());
    m_borderDensityComboBox->setCurrentIndex(m_borderDensityComboBox->findText(printer.getDefaultBorderDensity()));
    m_emptyDensityComboBox->addItems(printer.getAvailableEmptyImageDensityValues());
    m_emptyDensityComboBox->setCurrentIndex(m_emptyDensityComboBox->findText(printer.getDefaultEmptyImageDensity()));
    m_maximDensitySpinBox->setMaximum(printer.getAvailableMaxDensityValues());    
    m_maximDensitySpinBox->setValue(printer.getDefaultMaxDensity());
    m_miniumDensitySpinBox->setMaximum(printer.getAvailableMinDensityValues());
    m_miniumDensitySpinBox->setValue(printer.getDefaultMinDensity());
    m_visibleTrimCheckBox->setCheckable(printer.getAvailableTrim());
    m_visibleTrimCheckBox->setChecked(printer.getDefaultTrim());    
}

void QDicomPrinterConfigurationWidget::getDefaultPrinterSettingsFromControls(DicomPrinter& printer)
{   
    printer.setDefaultFilmLayout(m_layoutComboBox->currentText());
    printer.setDefaultFilmOrientation(m_filmOrientationComboBox->currentText());
    printer.setDefaultFilmSize(m_filmSizeComboBox->currentText());
    printer.setDefaultFilmDestination(m_filmDestinationComboBox->currentText());
    printer.setDefaultMagnificationType(m_magnifactionTypeComboBox->currentText());
    printer.setDefaultSmoothingType(m_smoothingTypeComboBox->currentText());
    printer.setDefaultBorderDensity(m_borderDensityComboBox->currentText());
    printer.setDefaultEmptyImageDensity(m_emptyDensityComboBox->currentText());
    printer.setDefaultMaxDensity(m_maximDensitySpinBox->value());
    printer.setDefaultMinDensity(m_miniumDensitySpinBox->value());
    printer.setDefaultTrim(m_visibleTrimCheckBox->isChecked());
}

void QDicomPrinterConfigurationWidget::setDefaultJobSettingsToControls(DicomPrinter& printer)
{   
    m_priorityComboBox->addItems(printer.getAvailablePrintPriorityValues());
    m_priorityComboBox->setCurrentIndex(m_priorityComboBox->findText(printer.getDefaultPrintPriority()));
    m_mediumTypeComboBox->addItems(printer.getAvailableMediumTypeValues());
    m_mediumTypeComboBox->setCurrentIndex(m_mediumTypeComboBox->findText(printer.getDefaultMediumType()));
}

void QDicomPrinterConfigurationWidget::getDefaultJobSettingsFromControls(DicomPrinter& printer)
{
    printer.setDefaultPrintPriority(m_priorityComboBox->currentText());   
    printer.setDefaultMediumType(m_mediumTypeComboBox->currentText());
}
}                                      