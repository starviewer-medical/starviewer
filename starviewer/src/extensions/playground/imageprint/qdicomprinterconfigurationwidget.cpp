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
    createActions();	
    createConnections();

    m_listPrintersTreeWidget->setColumnHidden(0,true); 
    clearPrinterParameters();
    fillPrintersList();
}

QDicomPrinterConfigurationWidget::~QDicomPrinterConfigurationWidget()
{

}

//*** Public Slot***//
void  QDicomPrinterConfigurationWidget::choosePrinter()
{
    DicomPrinterManager dicomPrinterManager;
    QTreeWidgetItem *selectedItem = 0;

    if( !m_listPrintersTreeWidget->selectedItems().isEmpty() )
    {   
        selectedItem = m_listPrintersTreeWidget->selectedItems().first();
        selectedPrinterId=selectedItem->text(0).toInt();
        DicomPrinter selectedPrinter = dicomPrinterManager.getPrinterByID(selectedPrinterId);
    
        // Printer
        m_printerAetitleLineEdit->setText(selectedPrinter.getAETitle());
        m_printerDescriptionLineEdit->setText(selectedPrinter.getDescription());
        m_printerHostnameLineEdit->setText(selectedPrinter.getHostname());
        m_printerPortLineEdit->setText(QString::number( selectedPrinter.getPort()));

        // Default Job Settings
        m_priorityComboBox->clear();
        m_priorityComboBox->addItems(selectedPrinter.getAvailablePrintPriorityValues());
        m_priorityComboBox->setCurrentIndex(m_priorityComboBox->findText(selectedPrinter.getDefaultPrintPriority()));
        m_mediumTypeComboBox->clear();
        m_mediumTypeComboBox->addItems(selectedPrinter.getAvailableMediumTypeValues());
        m_mediumTypeComboBox->setCurrentIndex(m_mediumTypeComboBox->findText(selectedPrinter.getDefaultMediumType()));

        // Default Printer Settings
        m_layoutComboBox->clear();
        m_layoutComboBox->addItems(selectedPrinter.getAvailableFilmLayoutValues());
        m_layoutComboBox->setCurrentIndex(m_layoutComboBox->findText(selectedPrinter.getDefaultFilmLayout()));    
        m_filmOrientationComboBox->clear();
        m_filmOrientationComboBox->addItems(selectedPrinter.getAvailableFilmOrientationValues());
        m_filmOrientationComboBox->setCurrentIndex(m_filmOrientationComboBox->findText(selectedPrinter.getDefaultFilmOrientation()));
        m_filmSizeComboBox->clear();
        m_filmSizeComboBox->addItems(selectedPrinter.getAvailableFilmSizeValues());
        m_filmSizeComboBox->setCurrentIndex(m_filmSizeComboBox->findText(selectedPrinter.getDefaultFilmSize()));
        m_filmDestinationComboBox->clear();
        m_filmDestinationComboBox->addItems(selectedPrinter.getAvailableFilmDestinationValues());
        m_filmDestinationComboBox->setCurrentIndex(m_filmDestinationComboBox->findText(selectedPrinter.getDefaultFilmDestination()));
        m_magnifactionTypeComboBox->clear();
        m_magnifactionTypeComboBox->addItems(selectedPrinter.getAvailableMagnificationTypeValues());
        m_magnifactionTypeComboBox->setCurrentIndex(m_magnifactionTypeComboBox->findText(selectedPrinter.getDefaultMagnificationType()));
        m_smoothingTypeComboBox->clear();
        m_smoothingTypeComboBox->addItems(selectedPrinter.getAvailableSmoothingTypeValues());
        m_smoothingTypeComboBox->setCurrentIndex(m_smoothingTypeComboBox->findText(selectedPrinter.getDefaultSmoothingType()));
        m_borderDensityComboBox->clear();
        m_borderDensityComboBox->addItems(selectedPrinter.getAvailableBorderDensityValues());
        m_borderDensityComboBox->setCurrentIndex(m_borderDensityComboBox->findText(selectedPrinter.getDefaultBorderDensity()));
        m_emptyDensityComboBox->clear();    
        m_emptyDensityComboBox->addItems(selectedPrinter.getAvailableEmptyImageDensityValues());
        m_emptyDensityComboBox->setCurrentIndex(m_emptyDensityComboBox->findText(selectedPrinter.getDefaultEmptyImageDensity()));
        m_maximDensitySpinBox->setMaximum(selectedPrinter.getAvailableMaxDensityValues());    
        m_maximDensitySpinBox->setValue(selectedPrinter.getDefaultMaxDensity());
        m_miniumDensitySpinBox->setMaximum(selectedPrinter.getAvailableMinDensityValues());
        m_miniumDensitySpinBox->setValue(selectedPrinter.getDefaultMinDensity());

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
    else
    {   
        clearPrinterParameters();
    }
}

void  QDicomPrinterConfigurationWidget::getAvailableParameters()
{
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

void QDicomPrinterConfigurationWidget::addPrinter()
{
    DicomPrinter dicomPrinter;
    DicomPrinterManager dicomPrinterManager;
    if (validatePrinterParameters())
    {   
        // Printer Parameters
        dicomPrinter.setAETitle(m_printerAetitleLineEdit->text());
        dicomPrinter.setDescription(m_printerDescriptionLineEdit->text());
        dicomPrinter.setHostname(m_printerHostnameLineEdit->text());
        dicomPrinter.setPort(m_printerPortLineEdit->text().toInt());
    
         // Default Job Settings
        dicomPrinter.setDefaultPrintPriority(m_priorityComboBox->currentText());   
        dicomPrinter.setDefaultMediumType(m_mediumTypeComboBox->currentText());

        // Default Printer Settings
        dicomPrinter.setDefaultFilmLayout(m_layoutComboBox->currentText());
        dicomPrinter.setDefaultFilmOrientation(m_filmOrientationComboBox->currentText());
        dicomPrinter.setDefaultFilmSize(m_filmSizeComboBox->currentText());
        dicomPrinter.setDefaultFilmDestination(m_filmDestinationComboBox->currentText());
        dicomPrinter.setDefaultMagnificationType(m_magnifactionTypeComboBox->currentText());
        dicomPrinter.setDefaultSmoothingType(m_smoothingTypeComboBox->currentText());
        dicomPrinter.setDefaultBorderDensity(m_borderDensityComboBox->currentText());
        dicomPrinter.setDefaultEmptyImageDensity(m_emptyDensityComboBox->currentText());
        dicomPrinter.setDefaultMaxDensity(m_maximDensitySpinBox->value());
        dicomPrinter.setDefaultMinDensity(m_miniumDensitySpinBox->value());
        dicomPrinter.setDefaultTrim(m_visibleTrimCheckBox->isChecked());

        if ( !dicomPrinterManager.addPrinter(dicomPrinter) )
        {
            QMessageBox::warning(this, ApplicationNameString, tr("This Printer already exists."));
        }
        else
        {
            fillPrintersList();
            clearPrinterParameters();
        }
    }
}

void QDicomPrinterConfigurationWidget::updatePrinter()
{
    DicomPrinter dicomPrinter;
    DicomPrinterManager dicomPrinterManager;
    if (validatePrinterParameters())
    {   
        // Printer Parameters
        dicomPrinter.setAETitle(m_printerAetitleLineEdit->text());
        dicomPrinter.setDescription(m_printerDescriptionLineEdit->text());
        dicomPrinter.setHostname(m_printerHostnameLineEdit->text());
        dicomPrinter.setPort(m_printerPortLineEdit->text().toInt());
    
         // Default Job Settings
        dicomPrinter.setDefaultPrintPriority(m_priorityComboBox->currentText());   
        dicomPrinter.setDefaultMediumType(m_mediumTypeComboBox->currentText());

        // Default Printer Settings
        dicomPrinter.setDefaultFilmLayout(m_layoutComboBox->currentText());
        dicomPrinter.setDefaultFilmOrientation(m_filmOrientationComboBox->currentText());
        dicomPrinter.setDefaultFilmSize(m_filmSizeComboBox->currentText());
        dicomPrinter.setDefaultFilmDestination(m_filmDestinationComboBox->currentText());
        dicomPrinter.setDefaultMagnificationType(m_magnifactionTypeComboBox->currentText());
        dicomPrinter.setDefaultSmoothingType(m_smoothingTypeComboBox->currentText());
        dicomPrinter.setDefaultBorderDensity(m_borderDensityComboBox->currentText());
        dicomPrinter.setDefaultEmptyImageDensity(m_emptyDensityComboBox->currentText());
        dicomPrinter.setDefaultMaxDensity(m_maximDensitySpinBox->value());
        dicomPrinter.setDefaultMinDensity(m_miniumDensitySpinBox->value());
        dicomPrinter.setDefaultTrim(m_visibleTrimCheckBox->isChecked());

        if (!dicomPrinterManager.updatePrinter(selectedPrinterId,dicomPrinter))
        {
            QMessageBox::warning(this, ApplicationNameString, tr("This Printer already exists."));
        }
        else
        {
            fillPrintersList();
            clearPrinterParameters();
        }
    }
}

void QDicomPrinterConfigurationWidget::deletePrinter()
{
    DicomPrinterManager dicomPrinterManager;
    dicomPrinterManager.deletePrinter(selectedPrinterId);
    this->fillPrintersList();
}

void QDicomPrinterConfigurationWidget::testPrinter()
{

}

//*** Private Methodes***//
void QDicomPrinterConfigurationWidget::createConnections()
{ 
	
}

void QDicomPrinterConfigurationWidget::createActions()
{
    connect( m_listPrintersTreeWidget , SIGNAL( itemSelectionChanged() ), SLOT( choosePrinter() ) );

    connect( m_addPrinterPushButton , SIGNAL( clicked() ), SLOT( addPrinter() ));
    connect( m_updatePrinterPushButton , SIGNAL( clicked() ), SLOT( updatePrinter() ));
    connect( m_deletePrinterPushButton , SIGNAL( clicked() ), SLOT( deletePrinter() ));
    connect( m_verifyPrinterPushButton , SIGNAL( clicked() ), SLOT( testPrinter() ));    

    connect( m_printerAetitleLineEdit, SIGNAL( textChanged(const QString &) ), SLOT( getAvailableParameters() ) );
    connect( m_printerPortLineEdit, SIGNAL( textChanged(const QString &) ), SLOT( getAvailableParameters() ) );
}

void QDicomPrinterConfigurationWidget::fillPrintersList()
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

void QDicomPrinterConfigurationWidget::clearPrinterParameters()
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

bool QDicomPrinterConfigurationWidget::validatePrinterParameters()
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
}                                      