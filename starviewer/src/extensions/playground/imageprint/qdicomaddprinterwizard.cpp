#include "qdicomaddprinterwizard.h"
#include <QMessageBox>
#include "starviewerapplication.h"
#include "dicomprinter.h"
#include "dicomprintermanager.h"

namespace udg {

QDicomAddPrinterWizard::QDicomAddPrinterWizard()
{
    setupUi( this );
    connect(this, SIGNAL(currentIdChanged ( int)),SLOT(addPrinter(int )));
}

QDicomAddPrinterWizard::~QDicomAddPrinterWizard()
{

}
void QDicomAddPrinterWizard::addPrinter(int codiPagina)
{   
    DicomPrinter dicomPrinter;
    DicomPrinterManager dicomPrinterManager;

    if(codiPagina==1)
    {         
        if(!this->validateInputSettings())
        {
            // S'ha de compravar si ja existex aquesta impressora. Si és que si, s'ha de mostrar un missatge d'error. Si no, 
            return;
        }

        this->getPrinterSettingsFromControls(dicomPrinter);
        if ( !dicomPrinterManager.addPrinter(dicomPrinter) )
        {
            m_titolPage2Label->setText(tr("Impressora no afegida pels següents errors: "));
            m_resultAddPrinterLabel->setText(tr("Existeix un altra impressora amb les mateixes característiques"));
            return;
        }

        m_titolPage2Label->setText(tr("Nova impressora afegida "));
        m_resultAddPrinterLabel->setText(tr("Felicitats, impressora afegida"));
    }
}


bool QDicomAddPrinterWizard::validateInputSettings()
{
    bool retorna;
    QString text;
    QString missatgeError;
    
    retorna = true;
    missatgeError = "";
    
    text = m_printerAetitleLineEdit->text();
    if(text.length()==0)
    {
        missatgeError =  missatgeError + tr("\nAETitle field can't be empty");
        retorna = false;
    }
    
    text = m_printerHostnameLineEdit->text();
    if(text.length()==0)
    {
        missatgeError = missatgeError + tr("\nHostname field can't be empty");        
        retorna = false;
    }
    
    text = m_printerPortLineEdit->text();
    if ( !( text.toInt( NULL , 10 ) >= 0 && text.toInt( NULL , 10 ) <= 65535 ) || text.length() ==0 )
    {
        missatgeError = missatgeError + tr("\nPrinter Port has to be between 0 and 65535");
        retorna = false;
    }

    if(!retorna)
    {
        m_titolPage2Label->setText(tr("Impressora no afegida pels següents errors: "));
        m_resultAddPrinterLabel->setText(missatgeError);
    }
    return retorna;
}

void QDicomAddPrinterWizard::getPrinterSettingsFromControls(DicomPrinter& printer)
{
    printer.setAETitle(m_printerAetitleLineEdit->text());
    printer.setDescription(m_printerDescriptionLineEdit->text());
    printer.setHostname(m_printerHostnameLineEdit->text());
    printer.setPort(m_printerPortLineEdit->text().toInt());
    printer.setIsDefault(m_printerDefaultPrinterCheckBox->isChecked());
}
}                                      