/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "qlistenrisrequestsconfigurationscreen.h"

#include <QIntValidator>
#include <QMessageBox>

#include "starviewerapplication.h"
#include "inputoutputsettings.h"

namespace udg {

QListenRisRequestsConfigurationScreen::QListenRisRequestsConfigurationScreen(QWidget *parent) : QWidget(parent)
{
    setupUi( this );

    loadRisConfiguration();
    m_buttonApplyListenRisConfiguration->setEnabled(false);

    this->setWindowIcon(QIcon(":images/RISConfiguration.png"));
    createConnections();
    m_buttonApplyListenRisConfiguration->setIcon(QIcon(":images/apply.png"));
    configureInputValidator();
}

QListenRisRequestsConfigurationScreen::~QListenRisRequestsConfigurationScreen()
{
}

void QListenRisRequestsConfigurationScreen::createConnections()
{
    connect(m_checkBoxListenRisRequests, SIGNAL(toggled(bool)), SLOT(enableApplyButtons()));
    connect(m_checkBoxViewAutomaticallyStudies, SIGNAL(toggled(bool)), SLOT(enableApplyButtons()));
    connect(m_textPortListenRisRequests, SIGNAL(textChanged(QString)), SLOT(enableApplyButtons()));

    connect(m_buttonApplyListenRisConfiguration, SIGNAL(clicked()), SLOT(applyChanges()));
}

void QListenRisRequestsConfigurationScreen::configureInputValidator()
{
    m_textPortListenRisRequests->setValidator(new QIntValidator(0, 99999, m_textPortListenRisRequests));
}

void QListenRisRequestsConfigurationScreen::loadRisConfiguration()
{
    Settings settings;

    bool listenToRISRequests = settings.getValue( InputOutputSettings::ListenToRISRequests).toBool();

    m_checkBoxListenRisRequests->setChecked( listenToRISRequests );
    m_checkBoxViewAutomaticallyStudies->setEnabled( listenToRISRequests );
    m_textPortListenRisRequests->setEnabled( listenToRISRequests );

    m_checkBoxViewAutomaticallyStudies->setChecked( settings.getValue( InputOutputSettings::RISRequestViewOnceRetrieved).toBool() );
    m_textPortListenRisRequests->setText( QString().setNum( settings.getValue( InputOutputSettings::RISRequestsPort).toInt() ) );
}

bool QListenRisRequestsConfigurationScreen::validateChanges()
{
    return 0 <= m_textPortListenRisRequests->text().toInt() && m_textPortListenRisRequests->text().toInt() <= 65535;
}

bool QListenRisRequestsConfigurationScreen::applyChanges()
{
    Settings settings;

    if (validateChanges())
    {
        if (m_textPortListenRisRequests->isModified() || m_checkBoxListenRisRequests->isChecked() != settings.getValue(InputOutputSettings::ListenToRISRequests).toBool() ) 
        {
            //S'ha de reiniciar en cas que iniciem/parem d'escoltar el port del RIS o canviem el port
            QMessageBox::warning( this , ApplicationNameString , tr( "The application has to be restarted to apply the changes." ) );
        }
        
        settings.setValue( InputOutputSettings::ListenToRISRequests, m_checkBoxListenRisRequests->isChecked() );
        if (m_textPortListenRisRequests->isModified())
        {
            settings.setValue( InputOutputSettings::RISRequestsPort, m_textPortListenRisRequests->text().toInt() );
            m_textPortListenRisRequests->setModified(false);//Indiquem que no s'ha modfiicat perquè ja hem guardat el seu valor
        }

        settings.setValue( InputOutputSettings::RISRequestViewOnceRetrieved, m_checkBoxViewAutomaticallyStudies->isChecked() );

        m_buttonApplyListenRisConfiguration->setDisabled(true);

        return true;
    }
    else return false;
}

void QListenRisRequestsConfigurationScreen::enableApplyButtons()
{
    m_buttonApplyListenRisConfiguration->setEnabled(true);
}


};
