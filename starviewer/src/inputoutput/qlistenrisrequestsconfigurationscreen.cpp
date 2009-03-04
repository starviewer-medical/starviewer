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
#include "starviewersettings.h"

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
    StarviewerSettings settings;

    m_checkBoxListenRisRequests->setChecked(settings.getListenRisRequests());
    m_checkBoxViewAutomaticallyStudies->setChecked(settings.getViewAutomaticallyAStudyRetrievedFromRisRequest());
    m_textPortListenRisRequests->setText(QString().setNum(settings.getListenPortRisRequests()));

    m_checkBoxViewAutomaticallyStudies->setEnabled(settings.getListenRisRequests());
    m_textPortListenRisRequests->setEnabled(settings.getListenRisRequests());
}

bool QListenRisRequestsConfigurationScreen::validateChanges()
{
    return 0 <= m_textPortListenRisRequests->text().toInt() && m_textPortListenRisRequests->text().toInt() <= 65535;
}

bool QListenRisRequestsConfigurationScreen::applyChanges()
{
    StarviewerSettings settings;

    if (validateChanges())
    {
        if (m_textPortListenRisRequests->isModified() || m_checkBoxListenRisRequests->isChecked() != settings.getListenRisRequests()) 
        {
            //S'ha de reiniciar en cas que iniciem/parem d'escoltar el port del RIS o canviem el port
            QMessageBox::warning( this , ApplicationNameString , tr( "The application has to be restarted to apply the changes." ) );
        }
        
        settings.setListenRisRequests(m_checkBoxListenRisRequests->isChecked());

        if (m_textPortListenRisRequests->isModified())
        {
            settings.setListenPortRisRequests(m_textPortListenRisRequests->text().toInt());
            m_textPortListenRisRequests->setModified(false);//Indiquem que no s'ha modfiicat perquè ja hem guardat el seu valor
        }

        settings.setViewAutomaticallyAStudyRetrievedFromRisRequest(m_checkBoxViewAutomaticallyStudies->isChecked());

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
