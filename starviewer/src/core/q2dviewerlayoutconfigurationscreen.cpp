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

#include "q2dviewerlayoutconfigurationscreen.h"

#include "coresettings.h"
#include "qlayoutoptionswidget.h"

#include <QMessageBox>

namespace udg {

Q2DViewerLayoutConfigurationScreen::Q2DViewerLayoutConfigurationScreen(QWidget *parent)
 : QWidget(parent)
{
    setupUi(this);
    initialize();

    connect(m_comparisonModeDivisionComboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [this](int index) {
        QString divisionString = CoreSettings::comparisonModeDivisionTypeToString(static_cast<CoreSettings::ComparisonModeDivisionType>(index));
        Settings().setValue(CoreSettings::ComparisonModeDivision, divisionString);
        QMessageBox::information(this, tr("Settings change"),
                                 tr("This change will take effect the next time the 2D viewer extension is opened or when another patient is loaded."));
    });
}

Q2DViewerLayoutConfigurationScreen::~Q2DViewerLayoutConfigurationScreen()
{
}

void Q2DViewerLayoutConfigurationScreen::initialize()
{
    QStringList modalities;
    modalities << "CR" << "CT" << "DX" << "ES" << "MG" << "MR" << "NM" << "OP" << "PT" << "RF" << "SC" << "US" << "XA" << "XC";
    foreach (const QString &modality, modalities)
    {
        m_modalitiesTabWidget->addTab(new QLayoutOptionsWidget(modality, m_modalitiesTabWidget), modality);
    }

    QString divisionSetting = Settings().getValue(CoreSettings::ComparisonModeDivision).toString();
    CoreSettings::ComparisonModeDivisionType division = CoreSettings::comparisonModeDivisionTypeFromString(divisionSetting);
    m_comparisonModeDivisionComboBox->setCurrentIndex(division);
}

}
