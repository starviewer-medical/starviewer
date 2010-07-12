/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "qreleasenotes.h"
#include "coresettings.h"
#include "starviewerapplication.h"

#include <QCloseEvent>
#include <QUrl>
#include <QFile>

namespace udg {

QReleaseNotes::QReleaseNotes(QWidget *parent)
 : QWidget(parent)
{
    setupUi(this);
    
    //no cal fer un metode a part per les connexions si només en tenim dos
    connect(m_closePushButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(m_viewWebView, SIGNAL(loadFinished(bool)), this, SLOT(checkLoadResult(bool)));

    //llegir el contingut dels fitxers HTML de les release notes
    m_viewWebView->setUrl(createUrl());
        
    setWindowTitle(tr("Release Notes"));
    setWindowModality(Qt::ApplicationModal);

    //per defecte diem que hem trobat la url
    m_urlNotFound = false;
    //no la hem comprobat
    m_urlChecked = false;
    //no volem mostrar la finestra
    m_intendToShow = false;
}

void QReleaseNotes::show()
{
    //ja hem comprobat la url
    if (m_urlChecked)
    {
        if (m_urlNotFound)
        {
            setVisible(false);
        }
        else
        {
            setVisible(true);
        }
    }
    else //encara no s'ha comprobat
    {
        m_intendToShow = true;
    }
}

void QReleaseNotes::checkLoadResult(bool result)
{
    //comprobar la url
    if (result == false)
    {
        m_urlNotFound = true;
    }
    else
    {
        //si tenim la intensió de mostrar la finestra
        if (m_intendToShow)
        {
            setVisible(true);
        }
    }

    m_urlChecked = true;
}

void QReleaseNotes::closeEvent(QCloseEvent *event)
{
    //no volem mostrar més la finestra
    m_intendToShow = false;
    //guardar els settings
    writeSettings();   
    //i tancar la finestra
    event->accept();
}

QUrl QReleaseNotes::createUrl()
{
    //agafar el path del fitxer
    QString defaultPath = qApp->applicationDirPath() + "/releasenotes/";
    if (!QFile::exists(defaultPath))
    {
        /// Mode desenvolupament
        defaultPath = qApp->applicationDirPath() + "/../releasenotes/";
    }

    QUrl result = defaultPath + "releasenotes" + udg::StarviewerVersionString + ".html";
    return result;
}

void QReleaseNotes::writeSettings()
{
    Settings *settings = new Settings();
    //ja no es mostren més fins la proxima actualització
    settings->setValue(CoreSettings::ShowReleaseNotesFirstTime, false);

    //primer de tot comprobar si el 'Don't show on future releases' esta activat
    if (m_dontShowCheckBox->isChecked())
    {
        //modificar els settings per que no es mostrin mai més  
        settings->setValue(CoreSettings::NeverShowReleaseNotes, true);
    }

    //destruir l'objecte settings
    delete settings;
}

}