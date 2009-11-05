#include <iostream>
#include "qdicomprinterconfigurationwidget.h"
#include "printer.h"
#include "dicomprinter.h"
#include "dicomprintermanager.h"
#include "starviewerapplication.h"
#include "qdicomaddprinterwidget.h"
#include "dicomprint.h"

#include <QMessageBox>

namespace udg {

// Public Methods
QDicomPrinterConfigurationWidget::QDicomPrinterConfigurationWidget()
{
    setupUi( this );

    m_listPrintersTreeWidget->setColumnHidden(0,true);

    m_addPrinterWidget = new QDicomAddPrinterWidget();    
    
    createConnections();
    refreshPrinterList();
    clearPrinterSettings();
    showAdvancedConfigurationOptions(false);
}

QDicomPrinterConfigurationWidget::~QDicomPrinterConfigurationWidget()
{

}


// Public Slots
void  QDicomPrinterConfigurationWidget::printerSelectionChanged()
{
    if(m_listPrintersTreeWidget->selectedItems().count() > 0)
    {   
        DicomPrinter selectedDicomPrinter = getSelectedDicomPrinter();

        this->clearPrinterSettings();
    
        this->setPrinterSettingsToControls(selectedDicomPrinter);
        this->setPrintSettingsToControls(selectedDicomPrinter);
        this->setFilmSettingsToControls(selectedDicomPrinter);
        this->setAdvancedSettingsToControls(selectedDicomPrinter);
    }
}

void QDicomPrinterConfigurationWidget::addPrinter()
{    
    m_addPrinterWidget->clearInputs();
    m_addPrinterWidget->setVisible(true);
}

bool QDicomPrinterConfigurationWidget::modifyPrinter()
{
    DicomPrinter dicomPrinter;
    DicomPrinterManager dicomPrinterManager;
    
    if (!m_listPrintersTreeWidget->selectedItems().isEmpty())
    {
        if (validatePrinterSettings())
        {   
            this->getPrinterSettingsFromControls(dicomPrinter);
            this->getPrintSettingsFromControls(dicomPrinter);
            this->getFilmSettingsFromControls(dicomPrinter);
            this->getAdvancedSettingsFromControls(dicomPrinter);

            if (!dicomPrinterManager.updatePrinter(m_selectedPrinterId,dicomPrinter))
            {
                //Si rebem un error és que no ha trobat la impressora amb el ID passat per paràmetre
                QMessageBox::critical(this, ApplicationNameString, tr("Error can't apply changes to the printer."));
            }
            else
            {
                refreshPrinterList();
                clearPrinterSettings();

                return true;
            }
        }
    }
    else
    {
        QMessageBox::information(this, ApplicationNameString, tr("A printer is not selected. To Add a new printer click at button '+'."));
    }

    return false;
}

void QDicomPrinterConfigurationWidget::deletePrinter()
{
    DicomPrinterManager dicomPrinterManager;
    dicomPrinterManager.deletePrinter(m_selectedPrinterId);
    this->refreshPrinterList();
    clearPrinterSettings();

}

void QDicomPrinterConfigurationWidget::testPrinter()
{
    if(!m_listPrintersTreeWidget->selectedItems().isEmpty())
    {   
        DicomPrint dicomPrint;
        DicomPrinter selectedDicomPrinter = getSelectedDicomPrinter();
        bool testIsCorrect;
        
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        testIsCorrect = dicomPrint.echoPrinter(selectedDicomPrinter);
        QApplication::restoreOverrideCursor();

        if (testIsCorrect)
        {
            //El test s'ha fet correctament i la impressora ha respós
            QMessageBox::information(this, ApplicationNameString, tr("Test of printer %1 is correct.").arg(selectedDicomPrinter.getAETitle()));
        }
        else
        {
            //El test ha fallat per algun motiu
            QString messageError;

            switch(dicomPrint.getLastError())
            {
                case DicomPrint::ErrorConnecting :
                    messageError = tr("Printer %1 doesn't respond.\nBe sure that the IP and AETitle are correct.").arg(selectedDicomPrinter.getAETitle());
                    break;
                case DicomPrint::NotRespondedAsExpected :
                    messageError = tr("Printer %1 doesn't respond correclty.\nBe sure that the IP and AETitle are correct." ).arg(selectedDicomPrinter.getAETitle());
                    break;
                default:
                    messageError = tr("Printer %1 doesn't respond.\nBe sure that the IP and AETitle are correct." ).arg(selectedDicomPrinter.getAETitle());
                    break;
            }

            QMessageBox::information(this, ApplicationNameString, messageError);
        }
    }
    else
    {
        QMessageBox::information( this , tr("Information") , tr("To test a Printer it is necessary to select an printer of the list.") );
    }

    DicomPrinter;
}

void QDicomPrinterConfigurationWidget::showAdvancedSettings()
{
    QString buttonText = tr("Advanced Settings");

    showAdvancedConfigurationOptions(!m_advancedSettingsGroupBox->isVisible());

    if (m_advancedSettingsGroupBox->isVisible())
    {
        m_advancedSettingsPushButton->setText(buttonText + " <<<");
    }
    else 
    {
        m_advancedSettingsPushButton->setText(buttonText + " >>>");
    }

}

void QDicomPrinterConfigurationWidget::showNewPrinterAdded()
{
    this->refreshPrinterList();
}

// Private Methods
void QDicomPrinterConfigurationWidget::createConnections()
{ 
    connect( m_listPrintersTreeWidget , SIGNAL( itemSelectionChanged() ), SLOT( printerSelectionChanged() ) );
    connect( m_addPrinterPushButton, SIGNAL( clicked() ), SLOT( addPrinter() ));
    connect( m_applySettingsPushButton , SIGNAL( clicked() ), SLOT( modifyPrinter() ));
    connect( m_acceptSettingsPushButton , SIGNAL( clicked() ), SLOT( accept() ));
    connect( m_cancelSettingsPushButton , SIGNAL( clicked() ), SLOT( cancel() ));
    connect( m_deletePrinterPushButton , SIGNAL( clicked() ), SLOT( deletePrinter() ));
    connect( m_testPrinterPushButton , SIGNAL( clicked() ), SLOT( testPrinter() ));    
    connect( m_advancedSettingsPushButton , SIGNAL( clicked() ), SLOT( showAdvancedSettings() ));   
    connect( m_addPrinterWidget, SIGNAL(newPrinterAddedSignal()), SLOT(showNewPrinterAdded()));
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
    m_yesVisibleTrimRadioButton->setChecked(true);

    // Advanced Settings
    m_magnifactionTypeComboBox->clear();
    m_smoothingTypeComboBox->clear();
    m_maximDensitySpinBox->setValue(0);
    m_miniumDensitySpinBox->setValue(0);
    m_polarityComboBox->clear();
    m_borderDensityComboBox->clear();
    m_emptyDensityComboBox->clear();
    m_configurationInformationLineEdit->setText("");
    m_printerDefaultPrinterCheckBox->setChecked(false);
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
    m_magnifactionTypeComboBox->addItems(printer.getAvailableMagnificationTypeValues());
    m_magnifactionTypeComboBox->setCurrentIndex(m_magnifactionTypeComboBox->findText(printer.getDefaultMagnificationType()));
    m_smoothingTypeComboBox->addItems(printer.getAvailableSmoothingTypeValues());
    m_smoothingTypeComboBox->setCurrentIndex(m_smoothingTypeComboBox->findText(printer.getDefaultSmoothingType()));
    m_maximDensitySpinBox->setMaximum(printer.getAvailableMaxDensityValues());
    m_maximDensitySpinBox->setValue(printer.getDefaultMaxDensity());
    m_polarityComboBox->addItems(printer.getAvailablePolarityValues());
    m_polarityComboBox->setCurrentIndex(m_polarityComboBox->findText(printer.getDefaultPolarity()));
    m_borderDensityComboBox->addItems(printer.getAvailableBorderDensityValues());
    m_borderDensityComboBox->setCurrentIndex(m_borderDensityComboBox->findText(printer.getDefaultBorderDensity()));
    m_emptyDensityComboBox->addItems(printer.getAvailableEmptyImageDensityValues());
    m_emptyDensityComboBox->setCurrentIndex(m_borderDensityComboBox->findText(printer.getDefaultEmptyImageDensity()));
    m_miniumDensitySpinBox->setMaximum(printer.getAvailableMinDensityValues());
    m_miniumDensitySpinBox->setValue(printer.getDefaultMinDensity());
    m_configurationInformationLineEdit->setText(printer.getConfigurationInformation());
}

void QDicomPrinterConfigurationWidget::getAdvancedSettingsFromControls(DicomPrinter& printer)
{
    printer.setDefaultMagnificationType(m_magnifactionTypeComboBox->currentText());
    printer.setDefaultSmoothingType(m_smoothingTypeComboBox->currentText());
    printer.setDefaultMaxDensity(m_maximDensitySpinBox->value());
    printer.setDefaultPolarity(m_polarityComboBox->currentText());
    printer.setDefaultBorderDensity(m_borderDensityComboBox->currentText());
    printer.setDefaultEmptyImageDensity(m_emptyDensityComboBox->currentText());
    printer.setDefaultMinDensity(m_miniumDensitySpinBox->value());
    printer.setConfigurationInformation(m_configurationInformationLineEdit->text());
}

DicomPrinter QDicomPrinterConfigurationWidget::getSelectedDicomPrinter()
{
    DicomPrinter selectedDicomPrinter;
    DicomPrinterManager dicomPrinterManager;
    QTreeWidgetItem *selectedItem;

    if (m_listPrintersTreeWidget->selectedItems().count() > 0)
    {
        /*Seleccionem només la primera impressora seleccionada. El QTreeWidget està configurat com SingleSelection, per tant només es pot seleccionar
          una impressora a la vegada*/
        selectedItem = m_listPrintersTreeWidget->selectedItems().first();
        m_selectedPrinterId = selectedItem->text(0).toInt();
        
        selectedDicomPrinter = dicomPrinterManager.getPrinterByID(m_selectedPrinterId);
    }

    return selectedDicomPrinter;
}

void QDicomPrinterConfigurationWidget::showAdvancedConfigurationOptions(bool show)
{
    m_advancedSettingsGroupBox->setVisible(show);
}

void QDicomPrinterConfigurationWidget::cancel()
{
    /*Deseleccionem l'impressora si tenim alguna seleccionada i netegem els controls, per si tornen a obrir la interfície de configuració,
     *com que no es crea i es destrueix cada vegada que es fa un show, es mostraria tal com estava abans de fer el cancel d'aquesta manera 
     sempre la mostrem no té cap impressora seleccionada i els controls no tenen valor, es mostra en el seu estat inicial*/
    m_listPrintersTreeWidget->clearSelection();
    clearPrinterSettings();
    
    close();
}

void QDicomPrinterConfigurationWidget::accept()
{
    bool closeWindow = false;

    if (m_listPrintersTreeWidget->selectedItems().count() > 0)
    {
        /*Si tenim una impressora seleccionada guardem possibles canvis que s'hagin fet, si es produeix algun error guardant els canvis, 
         no tanquem la finestra*/
        if (modifyPrinter())
        {
            closeWindow = true;
        }
    }
    else 
    {
        closeWindow = true;
    }

    if (closeWindow)
    {
        /*Deseleccionem l'impressora si tenim alguna seleccionada i netegem els controls, per si tornen a obrir la interfície de configuració,
         *com que no es crea i es destrueix cada vegada que es fa un show, es mostraria tal com estava abans de fer el cancel d'aquesta manera 
         sempre la mostrem no té cap impressora seleccionada i els controls no tenen valor, es mostra en el seu estat inicial*/

        m_listPrintersTreeWidget->clearSelection();
        clearPrinterSettings();
        close();
    }
}

}              
