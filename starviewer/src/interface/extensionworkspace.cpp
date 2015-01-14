/*************************************************************************************
  Copyright (C) 2014 Laboratori de Gràfics i Imatge, Universitat de Girona &
  Institut de Diagnòstic per la Imatge.
  Girona 2014. All rights reserved.
  http://starviewer.udg.edu

  This file is part of the Starviewer (Medical Imaging Software) open source project.
  It is subject to the license terms in the LICENSE file found in the top-level
  directory of this distribution and at http://starviewer.udg.edu/license. No part of
  the Starviewer (Medical Imaging Software) open source project, including this file,
  may be copied, modified, propagated, or distributed except according to the
  terms contained in the LICENSE file.
 *************************************************************************************/

#include "extensionworkspace.h"

#include "logging.h"

#include <QFile>
#include <QTextStream>

namespace udg {

ExtensionWorkspace::ExtensionWorkspace(QWidget *parent)
 : QTabWidget(parent)
{
    // Fem que cada pestanya tingui el seu botonet de tancar
    setTabsClosable(true);

    // QTabWidget takes much less time to be painted when DocumentMode is enabled
    setDocumentMode(true);

    this->setTabPosition(QTabWidget::South);
    this->setDarkBackgroundColorEnabled(true);

    createConnections();
}

ExtensionWorkspace::~ExtensionWorkspace()
{
}

void ExtensionWorkspace::createConnections()
{
    connect(this, SIGNAL(tabCloseRequested(int)), SLOT(closeApplicationByTabIndex(int)));
}

void ExtensionWorkspace::setDarkBackgroundColorEnabled(bool enabled)
{
    if (enabled)
    {
        QFile file(":css/mainwindowbackground.css");

        if (file.open(QFile::ReadOnly | QFile::Text))
        {
            QTextStream textStream(&file);
            this->setStyleSheet(textStream.readAll());
            file.close();
        }
    }
    else
    {
        this->setStyleSheet("");
    }
}

void ExtensionWorkspace::addApplication(QWidget *application, QString caption, const QString &extensionIdentifier)
{
    INFO_LOG("Afegim l'extensió: " + caption + " al workspace");
    this->addTab(application, caption);
    this->setCurrentIndex(this->indexOf(application));
    // Afegim l'extensió a la llista d'extensions actives
    m_activeExtensions.insert(application, extensionIdentifier);

    this->setDarkBackgroundColorEnabled(false);
}

void ExtensionWorkspace::removeApplication(QWidget *application)
{
    if (application)
    {
        INFO_LOG("Tancant extensió: " + application->objectName());
        this->removeTab(this->indexOf(application));
        // Eliminem l'extensió de la llista d'extensions actives
        m_activeExtensions.remove(application);

        if (m_activeExtensions.count() <= 0)
        {
            this->setDarkBackgroundColorEnabled(true);
        }
        // Esborrem de memòria l'extensió
        delete application;
    }
    else
    {
        DEBUG_LOG("S'ha donat una widget nul per eliminar");
    }
}

void ExtensionWorkspace::killThemAll()
{
    int numberOfExtensions = this->count();
    for (int i = numberOfExtensions - 1; i >= 0; i--)
    {
        QWidget *currentExtension = this->widget(i);
        removeApplication(currentExtension);
    }
}

QMap<QWidget *, QString> ExtensionWorkspace::getActiveExtensions() const
{
    return m_activeExtensions;
}

void ExtensionWorkspace::closeCurrentApplication()
{
    QWidget *w = this->currentWidget();
    removeApplication(w);
}

void ExtensionWorkspace::closeApplicationByTabIndex(int index)
{
    removeApplication(this->widget(index));
}

};  // end namespace udg
