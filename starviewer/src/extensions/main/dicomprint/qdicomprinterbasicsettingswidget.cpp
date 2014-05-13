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

#include <QWidget>

#include "qdicomprinterbasicsettingswidget.h"

#include "dicomprinter.h"

namespace udg {

QDicomPrinterBasicSettingsWidget::QDicomPrinterBasicSettingsWidget(QWidget *parent)
    : QWidget(parent)
{
    setupUi(this);
    createConnections();
}

void QDicomPrinterBasicSettingsWidget::createConnections()
{
    connect(m_priorityComboBox, SIGNAL(currentIndexChanged(int)), SLOT(settingChanged()));
    connect(m_mediumTypeComboBox, SIGNAL(currentIndexChanged(int)), SLOT(settingChanged()));
    connect(m_filmDestinationComboBox, SIGNAL(currentIndexChanged(int)), SLOT(settingChanged()));
    connect(m_layoutComboBox, SIGNAL(currentIndexChanged(int)), SLOT(settingChanged()));
    connect(m_filmOrientationComboBox, SIGNAL(currentIndexChanged(int)), SLOT(settingChanged()));
    connect(m_filmSizeComboBox, SIGNAL(currentIndexChanged(int)), SLOT(settingChanged()));
}

void QDicomPrinterBasicSettingsWidget::getDicomPrinterBasicSettings(DicomPrinter &dicomPrinter)
{
    getFilmSettings(dicomPrinter);
    getPrintSettings(dicomPrinter);
}

void QDicomPrinterBasicSettingsWidget::setDicomPrinterBasicSettings(DicomPrinter &dicomPrinter)
{
    setFilmSettings(dicomPrinter);
    setPrintSettings(dicomPrinter);
}

void QDicomPrinterBasicSettingsWidget::clear()
{
    // Print Settings
    m_priorityComboBox->clear();
    m_mediumTypeComboBox->clear();
    m_filmDestinationComboBox->clear();

    // Film Settings
    m_layoutComboBox->clear();
    m_filmOrientationComboBox->clear();
    m_filmSizeComboBox->clear();
}

void QDicomPrinterBasicSettingsWidget::getFilmSettings(DicomPrinter &printer)
{
    printer.setDefaultFilmLayout(m_layoutComboBox->currentText());
    printer.setDefaultFilmOrientation(m_filmOrientationComboBox->currentText());
    printer.setDefaultFilmSize(m_filmSizeComboBox->currentText());
}

void QDicomPrinterBasicSettingsWidget::getPrintSettings(DicomPrinter &printer)
{
    printer.setDefaultPrintPriority(m_priorityComboBox->currentText());
    printer.setDefaultMediumType(m_mediumTypeComboBox->currentText());
    printer.setDefaultFilmDestination(m_filmDestinationComboBox->currentText());
}

void QDicomPrinterBasicSettingsWidget::setFilmSettings(DicomPrinter &printer)
{
    m_layoutComboBox->addItems(printer.getAvailableFilmLayoutValues());
    m_layoutComboBox->setCurrentIndex(m_layoutComboBox->findText(printer.getDefaultFilmLayout()));
    m_filmOrientationComboBox->addItems(printer.getAvailableFilmOrientationValues());
    m_filmOrientationComboBox->setCurrentIndex(m_filmOrientationComboBox->findText(printer.getDefaultFilmOrientation()));
    m_filmSizeComboBox->addItems(printer.getAvailableFilmSizeValues());
    m_filmSizeComboBox->setCurrentIndex(m_filmSizeComboBox->findText(printer.getDefaultFilmSize()));
}

void QDicomPrinterBasicSettingsWidget::setPrintSettings(DicomPrinter &printer)
{
    m_priorityComboBox->addItems(printer.getAvailablePrintPriorityValues());
    m_priorityComboBox->setCurrentIndex(m_priorityComboBox->findText(printer.getDefaultPrintPriority()));
    m_mediumTypeComboBox->addItems(printer.getAvailableMediumTypeValues());
    m_mediumTypeComboBox->setCurrentIndex(m_mediumTypeComboBox->findText(printer.getDefaultMediumType()));
    m_filmDestinationComboBox->addItems(printer.getAvailableFilmDestinationValues());
    m_filmDestinationComboBox->setCurrentIndex(m_filmDestinationComboBox->findText(printer.getDefaultFilmDestination()));
}

void QDicomPrinterBasicSettingsWidget::settingChanged()
{
    emit basicDicomPrinterSettingChanged();
}

}
