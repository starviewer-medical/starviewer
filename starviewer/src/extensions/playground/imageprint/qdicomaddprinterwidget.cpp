#include <QMessageBox>
#include "qdicomaddprinterwidget.h"
#include "starviewerapplication.h"
#include "dicomprinter.h"
#include "dicomprintermanager.h"

namespace udg {

// Public Methods
QDicomAddPrinterWidget::QDicomAddPrinterWidget()
{
    setupUi( this );

    Qt::WindowFlags flags;
    flags = Qt::WindowMinimizeButtonHint;
    setWindowFlags( flags );

    this->createConnections();
}

QDicomAddPrinterWidget::~QDicomAddPrinterWidget()
{

}

void QDicomAddPrinterWidget::clearInputs()
{
    m_printerAetitleLineEdit->setText("");
    m_printerHostnameLineEdit->setText("");
    m_printerPortLineEdit->setText("");
    m_printerDescriptionLineEdit->setText("");
    
    m_addButton->setEnabled(false);
}

// Private Methods
void QDicomAddPrinterWidget::createConnections()
{   
    connect(m_addButton, SIGNAL(clicked()), SLOT(addPrinter()));
    connect(m_cancelButton, SIGNAL(clicked()), SLOT(close()));
    connect( m_printerAetitleLineEdit, SIGNAL( textChanged(const QString &) ), SLOT( enableAddButton() ) );
    connect( m_printerHostnameLineEdit, SIGNAL( textChanged(const QString &) ), SLOT( enableAddButton() ) );
    connect( m_printerPortLineEdit, SIGNAL( textChanged(const QString &) ), SLOT( enableAddButton() ) );
}

// Private Slots
void QDicomAddPrinterWidget::enableAddButton()
{
    QString text;

    text = m_printerAetitleLineEdit->text();
    if(text.length()==0)
    {
       m_addButton->setEnabled(false);
       return;
    }

    text = m_printerHostnameLineEdit->text();
    if(text.length()==0)
    {
       m_addButton->setEnabled(false);
       return;
    }
      
    text = m_printerPortLineEdit->text();
    if(text.length()==0)
    {
       m_addButton->setEnabled(false);
       return;
    }

    m_addButton->setEnabled(true);
}

void QDicomAddPrinterWidget::addPrinter()
{   
    DicomPrinter dicomPrinter;
    DicomPrinterManager dicomPrinterManager;
   
    if(this->validateInputSettings())
    {
      dicomPrinter = dicomPrinterManager.getAvailableParametersValues(m_printerAetitleLineEdit->text(),m_printerPortLineEdit->text().toInt()); 
      this->getPrinterSettingsFromControls(dicomPrinter);
      if ( !dicomPrinterManager.addPrinter(dicomPrinter) )
      {
        QMessageBox::critical(this, ApplicationNameString,  tr("Error: A printer with the same name already exists"));
      }
      else
      {
        QMessageBox::information(this, ApplicationNameString,  tr("New printer added"));
        emit newPrinterAddedSignal();
        this->close();
      }
    }
}

// Private Methods
bool QDicomAddPrinterWidget::validateInputSettings()
{       
    bool ok;
    int port;

    port = (m_printerPortLineEdit->text()).toInt(&ok,10);
    if ( !ok || port < 0 || port >65535)
    {   
        QMessageBox::critical(this, ApplicationNameString,  tr("\nPrinter Port has to be between 0 and 65535"));
        return false;
    }

    return true;
}

void QDicomAddPrinterWidget::getPrinterSettingsFromControls(DicomPrinter& printer)
{
    printer.setAETitle(m_printerAetitleLineEdit->text());
    printer.setHostname(m_printerHostnameLineEdit->text());
    printer.setPort(m_printerPortLineEdit->text().toInt());
    printer.setDescription(m_printerDescriptionLineEdit->text());
    printer.setIsDefault(m_printerDefaultPrinterCheckBox->isChecked());
}
}    