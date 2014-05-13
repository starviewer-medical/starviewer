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

#include "applicationstylehelper.h"

#include <QDesktopWidget>
#include <QApplication>
#include <QMovie>

#include "coresettings.h"
#include "mathtools.h"

namespace udg {

double ApplicationStyleHelper::m_scaleFactor = 1.0;

ApplicationStyleHelper::ApplicationStyleHelper()
{
}

void ApplicationStyleHelper::recomputeStyleToScreenOfWidget(QWidget *widget)
{
    // Per calcular l'style, calculem un factor d'escala a partir de la resolució de la pantalla a on s'està executant
    // el widget que ens passen. Aquest factor serà el que ens servirà per variar el tamany de les diferents fonts, etc.
    QDesktopWidget *desktop = QApplication::desktop();
    const QRect screen = desktop->screenGeometry(widget);

    if ((screen.width() * screen.height()) >= (5 * 1024 * 1024))
    {
        // Surt de fer 24/14 on 24 és el tamany de lletra desitjat i 14 és el "per defecte"
        m_scaleFactor = 1.72;
    }
    else if ((screen.width() * screen.height()) >= (3 * 1024 * 1024))
    {
        // Surt de fer 17/14
        m_scaleFactor = 1.22;
    }
    else
    {
        // Surt de fer 14/14
        m_scaleFactor = 1.0;
    }
}

int ApplicationStyleHelper::getToolsFontSize() const
{
    return this->getScaledFontSize(14.0, CoreSettings::ToolsFontSize);
}

void ApplicationStyleHelper::setScaledSizeTo(QMovie *movie) const
{
    QImage image(movie->fileName());
    movie->setScaledSize(image.size() * m_scaleFactor);
}

void ApplicationStyleHelper::setScaledFontSizeTo(QWidget *widget) const
{
    // Al ser text de Qt, agafem el tamany de font del sistema com a predeterminat
    int fontSize = this->getScaledFontSize(QApplication::font().pointSizeF(), CoreSettings::ScaledUserInterfaceFontSize);
    QString changeFontSize = QString("QLabel { font-size: %1pt }").arg(fontSize);
    widget->setStyleSheet(changeFontSize);
}

int ApplicationStyleHelper::getScaledFontSize(double defaultFontSize, const QString &settingsBackdoorKey) const
{
    double fontSize = defaultFontSize * m_scaleFactor;

    // TODO de moment es deixa un "backdoor" per poder especificar un text arbitrari a partir de configuració
    // caldrà treure'l un cop comprovat que no hi ha problemes
    Settings settings;
    if (settings.contains(settingsBackdoorKey))
    {
        fontSize = settings.getValue(settingsBackdoorKey).toInt();
    }

    return MathTools::roundToNearestInteger(fontSize);
}

} // End namespace udg
