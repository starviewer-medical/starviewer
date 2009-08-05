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

namespace udg {

QDICOMDIRConfigurationScreen::QDICOMDIRConfigurationScreen( QWidget *parent ) : QWidget(parent)
{
    setupUi( this );
    loadBurningDefaults();
    createConnections();
}

QDICOMDIRConfigurationScreen::~QDICOMDIRConfigurationScreen()
{
}

void QDICOMDIRConfigurationScreen::createConnections()
{
    //connecta el boto examinar programa de gravació amb el dialog per escollir el path del programa
    connect( m_buttonExaminateBurningApplication , SIGNAL( clicked() ), SLOT( examinateDICOMDIR() ) );
}

void QDICOMDIRConfigurationScreen::loadBurningDefaults()
{
    Settings settings;

    m_textBurningApplicationPath->setText(settings.getValue(InputOutputSettings::DICOMDIRBurningApplicationPathKey).toString());
    m_textBurningApplicationParameters->setText(settings.getValue(InputOutputSettings::DICOMDIRBurningApplicationParametersKey).toString());
}

bool QDICOMDIRConfigurationScreen::validateChanges()
{
    QDir dir;
    bool valid = true;

    QString messageBoxText = tr("Some configuration options are not valid:\n");

    if ( m_textBurningApplicationPath->isModified() )
    {
        if ( !dir.exists(m_textBurningApplicationPath->text() )) // si el fitxer indicat no existeix
        {
            QMessageBox::warning( this , ApplicationNameString , tr( "Invalid burning application path." ) );
            return false;
        }
    }

    if (!valid) QMessageBox::information(this, ApplicationNameString, messageBoxText);

    return valid;
}

bool QDICOMDIRConfigurationScreen::applyChanges()
{
    if (validateChanges())
    {
        applyChangesDICOMDIR();
        return true;
    }
    else 
    return false;
}

void QDICOMDIRConfigurationScreen::examinateDICOMDIR()
{
    //a la pàgina de QT indica que en el cas que nomes deixem seleccionar un fitxer, agafar el primer element de la llista i punt, no hi ha cap mètode que te retornin directament el fitxer selccionat
    QFileDialog *dlg = new QFileDialog( 0 , QFileDialog::tr( "Open" ) , "./" , ApplicationNameString + " Burning Application (*.exe)" );
    dlg->setFileMode( QFileDialog::ExistingFile );

    if ( dlg->exec() == QDialog::Accepted )
    {
        if ( !dlg->selectedFiles().empty() )
        {
            m_textBurningApplicationPath->setText( dlg->selectedFiles().takeFirst() );
            m_textBurningApplicationPath->setModified( true );// indiquem que m_textBurningApplicationPath ha modificat el seu valor
        }
    }
    delete dlg;
}

void QDICOMDIRConfigurationScreen::applyChangesDICOMDIR()
{
    Settings settings;

    settings.setValue(InputOutputSettings::DICOMDIRBurningApplicationPathKey, m_textBurningApplicationPath->text());
    settings.setValue(InputOutputSettings::DICOMDIRBurningApplicationParametersKey, m_textBurningApplicationParameters->text());

    if ( m_textBurningApplicationPath->isModified() )
    {
        INFO_LOG( "Es modificarà el path del programa de gravació per " + m_textBurningApplicationPath->text() );
    }

    if (m_textBurningApplicationParameters->isModified())
    {
        INFO_LOG("Es modificarà el parametres del programa de gravació per " + m_textBurningApplicationParameters->text() );
    }
}

};
