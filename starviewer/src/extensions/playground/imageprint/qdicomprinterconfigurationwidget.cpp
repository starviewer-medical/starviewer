#include <iostream>
#include "qdicomprinterconfigurationwidget.h"
#include "printer.h"
#include "dicomprinter.h"
#include "dicomprintermanager.h"
#include "starviewerapplication.h"
#include "qdicomaddprinterwizard.h"
#include <QMessageBox>


namespace udg {

QDicomPrinterConfigurationWidget::QDicomPrinterConfigurationWidget()
{
    setupUi( this );
    createConnections();

    m_listPrintersTreeWidget->setColumnHidden(0,true); 
    clearPrinterSettings();
    refreshPrinterList();
}

QDicomPrinterConfigurationWidget::~QDicomPrinterConfigurationWidget()
{

}


//*** Public Slot***//

void  QDicomPrinterConfigurationWidget::printerSelectionChanged()
{
    DicomPrinterManager dicomPrinterManager;
    QTreeWidgetItem *selectedItem = 0;
    
    this->clearPrinterSettings();

    if( !m_listPrintersTreeWidget->selectedItems().isEmpty() )
    {   
        selectedItem = m_listPrintersTreeWidget->selectedItems().first();
        m_selectedPrinterId = selectedItem->text(0).toInt();
        
        DicomPrinter selectedPrinter = dicomPrinterManager.getPrinterByID(m_selectedPrinterId);
    
        this->setPrinterSettingsToControls(selectedPrinter);
        this->setPrintSettingsToControls(selectedPrinter);
        this->setFilmSettingsToControls(selectedPrinter);
        this->setAdvancedSettingsToControls(selectedPrinter);
    }
}

void QDicomPrinterConfigurationWidget::addPrinter()
{
    
    QDicomAddPrinterWizard * prova = new QDicomAddPrinterWizard();
    prova->setVisible(true);

    /*
    DicomPrinter dicomPrinter;
    DicomPrinterManager dicomPrinterManager;
    if (validatePrinterSettings())
    {   
        this->getPrinterSettingsFromControls(dicomPrinter);
        this->getPrintSettingsFromControls(dicomPrinter);
        this->getFilmSettingsFromControls(dicomPrinter);
        this->getAdvancedSettingsFromControls(dicomPrinter);

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
    */
}

void QDicomPrinterConfigurationWidget::modifyPrinter()
{
    DicomPrinter dicomPrinter;
    DicomPrinterManager dicomPrinterManager;
    if (validatePrinterSettings())
    {   
        this->getPrinterSettingsFromControls(dicomPrinter);
        this->getPrintSettingsFromControls(dicomPrinter);
        this->getFilmSettingsFromControls(dicomPrinter);
        this->getAdvancedSettingsFromControls(dicomPrinter);

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

void QDicomPrinterConfigurationWidget::showAdvancedSettings()
{
}

void  QDicomPrinterConfigurationWidget::getAvailableParameters()
{
    DicomPrinterManager dicomPrinterManager;
    DicomPrinter printer = dicomPrinterManager.getAvailableParametersValues(m_printerAetitleLineEdit->text(),(m_printerPortLineEdit->text().toInt()));
    
    this->clearPrinterSettings();

    this->setPrinterSettingsToControls(printer);
    this->setPrintSettingsToControls(printer);
    this->setFilmSettingsToControls(printer);
    this->setAdvancedSettingsToControls(printer);
}

//*** Private Methods***//

void QDicomPrinterConfigurationWidget::createConnections()
{ 
    connect( m_listPrintersTreeWidget , SIGNAL( itemSelectionChanged() ), SLOT( printerSelectionChanged() ) );

    connect(m_addPrinterPushButton, SIGNAL( clicked() ), SLOT( addPrinter() ));
    connect( m_modifyPushButton , SIGNAL( clicked() ), SLOT( modifyPrinter() ));
    connect( m_deletePrinterPushButton , SIGNAL( clicked() ), SLOT( deletePrinter() ));
    connect( m_testPushButton , SIGNAL( clicked() ), SLOT( testPrinter() ));    
    connect( m_advancedSettingsPushButton , SIGNAL( clicked() ), SLOT( showAdvancedSettings() ));    
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
    // Printer Settings
    m_printerAetitleLineEdit->setText("");
    m_printerDescriptionLineEdit->setText("");
    m_printerHostnameLineEdit->setText("");
    m_printerPortLineEdit->setText("");

    // Print Settings
    m_numberCopiesSpinBox->setValue(0);
    m_priorityComboBox->clear();
    m_mediumTypeComboBox->clear();
    m_filmDestinationComboBox->clear();
    
    // Film Settings
    m_layoutComboBox->clear();
    m_filmOrientationComboBox->clear();        
    m_filmSizeComboBox->clear();
    m_visibleTrimCheckBox->clear();

    // Advanced Settings

}

void QDicomPrinterConfigurationWidget::setPrinterSettingsToControls(DicomPrinter& printer)
{
    m_printerAetitleLineEdit->setText(printer.getAETitle());
    m_printerDescriptionLineEdit->setText(printer.getDescription());
    m_printerHostnameLineEdit->setText(printer.getHostname());
    m_printerPortLineEdit->setText(QString::number( printer.getPort()));
    m_printerDefaultPrinterCheckBox->setChecked(printer.getIsDefault());
}

void QDicomPrinterConfigurationWidget::getPrinterSettingsFromControls(DicomPrinter& printer)
{
    printer.setAETitle(m_printerAetitleLineEdit->text());
    printer.setDescription(m_printerDescriptionLineEdit->text());
    printer.setHostname(m_printerHostnameLineEdit->text());
    printer.setPort(m_printerPortLineEdit->text().toInt());
    printer.setIsDefault(m_printerDefaultPrinterCheckBox->isChecked());
}

void QDicomPrinterConfigurationWidget::setPrintSettingsToControls(DicomPrinter& printer)
{
    m_priorityComboBox->addItems(printer.getAvailablePrintPriorityValues());
    m_priorityComboBox->setCurrentIndex(m_priorityComboBox->findText(printer.getDefaultPrintPriority()));
    m_mediumTypeComboBox->addItems(printer.getAvailableMediumTypeValues());
    m_mediumTypeComboBox->setCurrentIndex(m_mediumTypeComboBox->findText(printer.getDefaultMediumType()));
    m_filmDestinationComboBox->addItems(printer.getAvailableFilmDestinationValues());
    m_filmDestinationComboBox->setCurrentIndex(m_filmDestinationComboBox->findText(printer.getDefaultFilmDestination()));
}

void QDicomPrinterConfigurationWidget::getPrintSettingsFromControls(DicomPrinter& printer)
{
    printer.setDefaultPrintPriority(m_priorityComboBox->currentText());   
    printer.setDefaultMediumType(m_mediumTypeComboBox->currentText());
    printer.setDefaultFilmDestination(m_filmDestinationComboBox->currentText());
}

void QDicomPrinterConfigurationWidget::setFilmSettingsToControls(DicomPrinter& printer)
{
    m_layoutComboBox->addItems(printer.getAvailableFilmLayoutValues());
    m_layoutComboBox->setCurrentIndex(m_layoutComboBox->findText(printer.getDefaultFilmLayout()));    
    m_filmOrientationComboBox->addItems(printer.getAvailableFilmOrientationValues());
    m_filmOrientationComboBox->setCurrentIndex(m_filmOrientationComboBox->findText(printer.getDefaultFilmOrientation()));
    m_filmSizeComboBox->addItems(printer.getAvailableFilmSizeValues());
    m_filmSizeComboBox->setCurrentIndex(m_filmSizeComboBox->findText(printer.getDefaultFilmSize()));
}

void QDicomPrinterConfigurationWidget::getFilmSettingsFromControls(DicomPrinter& printer)
{
    printer.setDefaultFilmLayout(m_layoutComboBox->currentText());
    printer.setDefaultFilmOrientation(m_filmOrientationComboBox->currentText());
    printer.setDefaultFilmSize(m_filmSizeComboBox->currentText());
}

void QDicomPrinterConfigurationWidget::setAdvancedSettingsToControls(DicomPrinter& printer)
{

}

void QDicomPrinterConfigurationWidget::getAdvancedSettingsFromControls(DicomPrinter& printer)
{

}
}                                      