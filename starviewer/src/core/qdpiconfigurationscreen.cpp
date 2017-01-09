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


#include "coresettings.h"
#include "qdpiconfigurationscreen.h"

#include "coresettings.h"

#include <QCloseEvent>
#include <QApplication>
#include <QMessageBox>
#include <QScreen>

namespace udg {

QDPIConfigurationScreen::QDPIConfigurationScreen(QWidget *parent)
 : QWidget(parent)
{
    setupUi(this);

    m_devToPixelRatio = ((QGuiApplication*)(QApplication::instance()))->devicePixelRatio();
    m_devDPI = ((QGuiApplication*)(QApplication::instance()))->primaryScreen()->logicalDotsPerInch();

    m_smallReference = this->studyIcon->height() / m_devToPixelRatio;
    m_mediumReference = this->axialIcon->height() / m_devToPixelRatio;
    m_largeReference = this->logoIcon->height() / m_devToPixelRatio;
    m_fontReference = logoLabel->font().pointSizeF() / m_devToPixelRatio;

    m_factor = 0;

    // Read configuration
    {
        QVariant cfgValue = udg::Settings().getValue(udg::CoreSettings::ScaleFactor);
        bool exists;
        int scaleFactor = cfgValue.toInt(&exists);
        if (exists && scaleFactor != 0) { // Setting exists and is different than one
            m_factor = scaleFactor;
        }
    }
    m_initialFactor = m_factor;

    factorSlider->setValue(m_factor);

    connect(enableCheckbox, SIGNAL(toggled(bool)), this, SLOT(checkboxChanged(bool)));
    connect(factorSlider, SIGNAL(valueChanged(int)), this, SLOT(factorChanged(int)));

    if (m_factor != 0) {
        enableCheckbox->setChecked(true);
    }

    // Enabled or not, prevew is rescaled and DPIs are calculated
    factorChanged(m_factor);
}

QDPIConfigurationScreen::~QDPIConfigurationScreen()
{

}

void QDPIConfigurationScreen::closeEvent(QCloseEvent *event)
{
    udg::Settings settings;
    settings.setValue(udg::CoreSettings::ScaleFactor, m_factor);
    if (m_factor != m_initialFactor) {
        QMessageBox::information(this,"Restart required", "In order to see the magnification changes you must restart Starviewer.");
    }

    event->accept();
}

void QDPIConfigurationScreen::checkboxChanged(bool enabled)
{
    if (!enabled)
    {
        m_factor = 0;
        factorSlider->setValue(m_factor);
    }
    dpiLabel->setEnabled(enabled);
    factorSlider->setEnabled(enabled);
    previewGroupBox->setEnabled(enabled);
}

void QDPIConfigurationScreen::factorChanged(int value)
{
    m_factor = value;
    qreal multiplier = 1 + (value * 0.125);

    dpiLabel->setText(QString("%1x (%2 dpi)").arg(multiplier, 0, 'f', 3).arg(m_devDPI * multiplier, 0, 'f', 0));


    qreal smallSize = m_smallReference * multiplier;
    qreal mediumSize = m_mediumReference * multiplier;
    qreal largeSize = m_largeReference * multiplier;
    qreal fontSize = m_fontReference * multiplier;
    QFont font = logoLabel->font();
    font.setPointSizeF(fontSize);

    studyIcon->setMinimumSize(smallSize,smallSize);
    studyIcon->setMaximumSize(smallSize,smallSize);
    studyLabel->setFont(font);
    seriesIcon->setMinimumSize(smallSize,smallSize);
    seriesIcon->setMaximumSize(smallSize,smallSize);
    seriesLabel->setFont(font);
    dicomIcon->setMinimumSize(smallSize,smallSize);
    dicomIcon->setMaximumSize(smallSize,smallSize);
    dicomLabel->setFont(font);

    axialIcon->setMinimumSize(mediumSize,mediumSize);
    axialIcon->setMaximumSize(mediumSize,mediumSize);
    axialLabel->setFont(font);
    sagittalIcon->setMinimumSize(mediumSize,mediumSize);
    sagittalIcon->setMaximumSize(mediumSize,mediumSize);
    sagittalLabel->setFont(font);
    coronalIcon->setMinimumSize(mediumSize,mediumSize);
    coronalIcon->setMaximumSize(mediumSize,mediumSize);
    coronalLabel->setFont(font);

    logoIcon->setMinimumSize(largeSize,largeSize);
    logoIcon->setMaximumSize(largeSize,largeSize);
    logoLabel->setFont(font);
}



}
