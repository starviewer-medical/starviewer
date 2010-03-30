/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "qdicomdirconfigurationscreen.h"

#include <QMessageBox>
#include <QFileDialog>

#include "starviewerapplication.h"
#include "inputoutputsettings.h"
#include "logging.h"
#include "settings.h"
#include "converttodicomdir.h"

namespace udg {

QDICOMDIRConfigurationScreen::QDICOMDIRConfigurationScreen( QWidget *parent ) : QWidget(parent)
{
    setupUi( this );
    loadDICOMDIRDefaults();
    createConnections();
}

QDICOMDIRConfigurationScreen::~QDICOMDIRConfigurationScreen()
{
}

void QDICOMDIRConfigurationScreen::createConnections()
{
    // Connecta el boto examinar programa de gravació amb el dialog per escollir el path del programa
    connect( m_buttonExaminateBurningApplication , SIGNAL( clicked() ), SLOT( examinateDICOMDIRBurningApplicationPath() ) );
    connect( m_buttonExaminateDICOMDIRFolderPathToCopy , SIGNAL( clicked() ), SLOT( examinateDICOMDIRFolderPathToCopy() ) );

    connect( m_checkBoxCopyFolderContentToDICOMDIRCdDvd , SIGNAL( toggled(bool) ), SLOT ( checkBoxCopyFolderContentToDICOMDIRToggled() ) );
    connect( m_checkBoxCopyFolderContentToDICOMDIRUsbHardDisk , SIGNAL( toggled(bool) ), SLOT ( checkBoxCopyFolderContentToDICOMDIRToggled() ) );
}

void QDICOMDIRConfigurationScreen::loadDICOMDIRDefaults()
{
    Settings settings;

    m_textBurningApplicationPath->setText( settings.getValue(InputOutputSettings::DICOMDIRBurningApplicationPathKey).toString() );
    m_textBurningApplicationParameters->setText( settings.getValue(InputOutputSettings::DICOMDIRBurningApplicationParametersKey).toString() );
    m_textBurningApplicationCDParameters->setText( settings.getValue(InputOutputSettings::DICOMDIRBurningApplicationCDParametersKey).toString() );
    m_textBurningApplicationDVDParameters->setText( settings.getValue(InputOutputSettings::DICOMDIRBurningApplicationDVDParametersKey).toString() );

    bool hasDifferentCDDVDParameters = settings.getValue( InputOutputSettings::DICOMDIRBurningApplicationHasDifferentCDDVDParametersKey).toBool();

    m_textBurningApplicationCDParameters->setEnabled( hasDifferentCDDVDParameters );
    m_textBurningApplicationDVDParameters->setEnabled( hasDifferentCDDVDParameters );
    m_checkBoxHasDifferentCDDVDParameteres->setChecked( hasDifferentCDDVDParameters );

    m_checkBoxConvertDICOMDIRImagesToLittleEndian->setChecked( settings.getValue(InputOutputSettings::ConvertDICOMDIRImagesToLittleEndianKey).toBool() );

    if( m_textBurningApplicationPath->text().endsWith("k3b") )
    {
        m_burningApplicationDownloadURL->setText("<a href='http://k3b.plainblack.com'> k3b.plainblack.com </a>");
    }     
    else
    {
        if( m_textBurningApplicationPath->text().endsWith("ImgBurn.exe") )
        {        
            m_burningApplicationDownloadURL->setText("<a href='http://www.imgburn.com'> www.imgburn.com </a>");
        }         
        else
        {
            if( m_textBurningApplicationPath->text().endsWith("Burn") )
            {            
                m_burningApplicationDownloadURL->setText("<a href='http://burn-osx.sourceforge.net'> burn-osx.sourceforge.net </a>");
            }
            else
            {
                m_burningApplicationDownloadURLLabel->setVisible(false);
                m_burningApplicationDownloadURL->setVisible(false);
            }
        }
    }

    m_checkBoxCopyFolderContentToDICOMDIRCdDvd->setChecked(settings.getValue(InputOutputSettings::CopyFolderContentToDICOMDIRCdDvd).toBool());
    m_checkBoxCopyFolderContentToDICOMDIRUsbHardDisk->setChecked(settings.getValue(InputOutputSettings::CopyFolderContentToDICOMDIRUsbHardDisk).toBool());
    m_textDICOMDIRFolderPathToCopy->setText(settings.getValue(InputOutputSettings::DICOMDIRFolderPathToCopy).toString());
}

bool QDICOMDIRConfigurationScreen::validateChanges()
{
    bool valid = true;

    QString messageBoxText = tr("Some configuration options are not valid:\n");

    if ( m_textBurningApplicationPath->isModified() )
    {
        if ( !QFile::exists(m_textBurningApplicationPath->text()) ) // Si el fitxer indicat no existeix
        {
            QMessageBox::warning( this , ApplicationNameString , tr( "Invalid burning application path." ) );
            return false;
        }

        QFileInfo burningApplicationPathInfo( m_textBurningApplicationPath->text() );
        if ( !burningApplicationPathInfo.isExecutable() )
        {
            QMessageBox::warning( this , ApplicationNameString , tr( "Burning application path has to be an executable file." ) );
            return false;
        }
    }

    if ( m_checkBoxCopyFolderContentToDICOMDIRCdDvd->isChecked() || m_checkBoxCopyFolderContentToDICOMDIRUsbHardDisk->isChecked() )
    {
        //Si ens han indiquen que s'ha de copiar el contingut del DICOMDIR en algun dispostiu és obligatori indicar el Path
        if ( m_textDICOMDIRFolderPathToCopy->text().isEmpty() )
        {
            QMessageBox::warning( this, ApplicationNameString, tr( "You have to indicate the path of folder to copy to DICOMDIR.") );
            return false;
        }

    }

    if ( m_textDICOMDIRFolderPathToCopy->isModified() && !m_textDICOMDIRFolderPathToCopy->text().isEmpty() )
    {
        if ( !QFile::exists( m_textDICOMDIRFolderPathToCopy->text() ) )
        {
            QMessageBox::warning( this, ApplicationNameString, tr( "Invalid path of folder to copy to DICOMDIR.") );
            return false;
        }
        else if ( !ConvertToDicomdir().AreValidRequirementsOfFolderContentToCopyToDICOMDIR( m_textDICOMDIRFolderPathToCopy->text() ) )
        {//Comprovem que el directori no tingui cap item que es digui DICOM o DICOMDIR
            QMessageBox::warning( this, ApplicationNameString, tr( "Invalid content of the folder to copy to DICOMDIR, This folder can't contains any item called DICOM or DICOMDIR.") );
            return false;
        }
    }

    return valid;
}

bool QDICOMDIRConfigurationScreen::applyChanges()
{
    if ( validateChanges() )
    {
        applyChangesDICOMDIR();
        return true;
    }
    else
    {
        return false;
    }
}

void QDICOMDIRConfigurationScreen::examinateDICOMDIRBurningApplicationPath()
{
    Settings settings;

    // A la pàgina de QT indica que en el cas que nomes deixem seleccionar un fitxer, agafar el primer element de la llista i punt, no hi ha cap mètode que te retornin directament el fitxer selccionat
    QFileDialog *dialog = new QFileDialog( 0 , QFileDialog::tr( "Open" ) , settings.getValue(InputOutputSettings::DICOMDIRBurningApplicationPathKey).toString(), "" );
    dialog->setFileMode( QFileDialog::ExistingFile );

    if ( dialog->exec() == QDialog::Accepted )
    {
        if ( !dialog->selectedFiles().empty() )
        {
            QString burningApplicationPath = dialog->selectedFiles().takeFirst();
            QFileInfo infoBurningApplicationFile( burningApplicationPath );
            // Es comprova si es tracta d'una aplicació de Mac i en cas afirmatiu es modifica el path per tal d'indicar exactament on és l'executable
            if( infoBurningApplicationFile.isBundle() )
            {
                // El path es treu una mica a lo "bruto". Per fer-ho bé s'hauria de llegir el Bundle i extreure'n
                // la localització de l'executable (CFBundleExecutable): http://developer.apple.com/documentation/CoreFoundation/Conceptual/CFBundles/BundleTypes/BundleTypes.html
                burningApplicationPath = burningApplicationPath + "/Contents/MacOS/" + infoBurningApplicationFile.bundleName();
            }

            m_textBurningApplicationPath->setText( burningApplicationPath );
            m_textBurningApplicationPath->setModified( true );// indiquem que m_textBurningApplicationPath ha modificat el seu valor
        }
    }
    delete dialog;
}

void QDICOMDIRConfigurationScreen::examinateDICOMDIRFolderPathToCopy()
{
    QString folderPathToCopy = QFileDialog::getExistingDirectory(0, tr("Folder to copy to DICOMDIR..."), m_textDICOMDIRFolderPathToCopy->text() );

    if (!folderPathToCopy.isEmpty())
    {
        m_textDICOMDIRFolderPathToCopy->setText( QDir::toNativeSeparators(folderPathToCopy) );
    }
}

void QDICOMDIRConfigurationScreen::applyChangesDICOMDIR()
{
    Settings settings;

    settings.setValue( InputOutputSettings::DICOMDIRBurningApplicationPathKey, m_textBurningApplicationPath->text() );
    settings.setValue( InputOutputSettings::DICOMDIRBurningApplicationParametersKey, m_textBurningApplicationParameters->text() );
    settings.setValue( InputOutputSettings::DICOMDIRBurningApplicationCDParametersKey, m_textBurningApplicationCDParameters->text() );
    settings.setValue( InputOutputSettings::DICOMDIRBurningApplicationDVDParametersKey, m_textBurningApplicationDVDParameters->text() );
    settings.setValue( InputOutputSettings::DICOMDIRBurningApplicationHasDifferentCDDVDParametersKey, m_checkBoxHasDifferentCDDVDParameteres->isChecked() );
    settings.setValue( InputOutputSettings::ConvertDICOMDIRImagesToLittleEndianKey, m_checkBoxConvertDICOMDIRImagesToLittleEndian->isChecked() );
    settings.setValue( InputOutputSettings::DICOMDIRFolderPathToCopy, m_textDICOMDIRFolderPathToCopy->text() );
    settings.setValue( InputOutputSettings::CopyFolderContentToDICOMDIRCdDvd, m_checkBoxCopyFolderContentToDICOMDIRCdDvd->isChecked() );
    settings.setValue( InputOutputSettings::CopyFolderContentToDICOMDIRUsbHardDisk, m_checkBoxCopyFolderContentToDICOMDIRUsbHardDisk->isChecked() );

    if ( m_textBurningApplicationPath->isModified() )
    {
        INFO_LOG( "Es modificarà el path del programa de gravació per " + m_textBurningApplicationPath->text() );
    }

    if ( m_textBurningApplicationParameters->isModified() )
    {
        INFO_LOG( "Es modificarà el parametres del programa de gravació per " + m_textBurningApplicationParameters->text() );
    }
    
    if ( m_textBurningApplicationCDParameters->isModified() )
    {
        INFO_LOG( "Es modificarà el parametres del programa de gravació (referent a la gravació en CD) per " + m_textBurningApplicationCDParameters->text() );
    }

    if ( m_textBurningApplicationDVDParameters->isModified() )
    {
        INFO_LOG( "Es modificarà el parametres del programa de gravació (referent a la gravació en DVD) per " + m_textBurningApplicationDVDParameters->text() );
    }

    if ( m_textDICOMDIRFolderPathToCopy->isModified() )
    {
        INFO_LOG( "Es modificarà el path del directori a copiar al DICOMDIR " + m_textDICOMDIRFolderPathToCopy->text() );
    }

}

};
