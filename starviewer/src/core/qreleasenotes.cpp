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

#include "qreleasenotes.h"

#include "coresettings.h"
#include "logging.h"

#include <QCloseEvent>
#include <QUrl>

namespace udg {

QReleaseNotes::QReleaseNotes(QWidget *parent)
 : QWidget(parent)
{
    setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);

    // No cal fer un metode a part per les connexions si només en tenim una
    connect(m_closePushButton, SIGNAL(clicked()), this, SLOT(close()));

    // Fer que la finestra sempre quedi davant i no es pugui fer res fins que no es tanqui
    setWindowModality(Qt::ApplicationModal);

    m_textBrowser->setContextMenuPolicy(Qt::NoContextMenu);
}

QReleaseNotes::~QReleaseNotes()
{
}

bool QReleaseNotes::isDontShowAnymoreChecked()
{
    return m_dontShowCheckBox->isChecked();
}

void QReleaseNotes::setDontShowVisible(bool visible)
{
    m_dontShowCheckBox->setVisible(visible);
}

void QReleaseNotes::showIfUrlLoadsSuccessfully(const QUrl &url)
{
    connect(m_textBrowser, &QTextBrowser::sourceChanged, this, &QReleaseNotes::loadFinished);
    m_textBrowser->setSource(url);
}

void QReleaseNotes::closeEvent(QCloseEvent *event)
{
    Settings settings;
    // Si el 'Don't show on future releases' esta marcat
    if (isDontShowAnymoreChecked())
    {
        // Modificar els settings per que no es mostrin mai més
        settings.setValue(CoreSettings::NeverShowNewVersionReleaseNotes, true);
    }
    // I tancar la finestra
    event->accept();
}

void QReleaseNotes::loadFinished()
{
    // Desconectar el manager
    disconnect(m_textBrowser, &QTextBrowser::sourceChanged, this, &QReleaseNotes::loadFinished);

    m_textBrowser->clearHistory();
    show();
}

} // End namespace udg
