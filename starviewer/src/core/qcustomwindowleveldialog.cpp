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

#include "qcustomwindowleveldialog.h"
#include "logging.h"

namespace udg {

QCustomWindowLevelDialog::QCustomWindowLevelDialog(QDialog *parent)
 : QDialog(parent)
{
    setupUi(this);
    createConnections();
}

QCustomWindowLevelDialog::~QCustomWindowLevelDialog()
{
}

void QCustomWindowLevelDialog::setDefaultWindowLevel(double window, double level)
{
    m_windowSpinBox->setValue(window);
    m_levelSpinBox->setValue(level);
}

void QCustomWindowLevelDialog::createConnections()
{
    connect(m_okButton, SIGNAL(clicked()), this, SLOT(confirmWindowLevel()));
    connect(m_cancelButton, SIGNAL(clicked()), this, SLOT(close()));
}

void QCustomWindowLevelDialog::confirmWindowLevel()
{
    // Validar els spin box
    if (m_windowSpinBox->value())
    {
        // \TODO implement me!
        DEBUG_LOG("Falta validar el valor del window");
    }
    if (m_levelSpinBox->value())
    {
        // \TODO implement me!
        DEBUG_LOG("Falta validar el valor del level");
    }

    emit windowLevel(m_windowSpinBox->value(), m_levelSpinBox->value());
    this->close();
}

};
