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

#include "q2dviewerannotationsconfigurationwidget.h"

#include "q2dviewerannotationssettingshelper.h"

namespace udg {

Q2DViewerAnnotationsConfigurationWidget::Q2DViewerAnnotationsConfigurationWidget(const QString &modality, QWidget *parent)
    : QWidget(parent), m_modality(modality), m_validatingModality(false), m_modalityValidated(true)
{
    setupUi(this);

    m_topLeftPlainTextEdit->setAlignment(Qt::AlignLeft);
    m_topRightPlainTextEdit->setAlignment(Qt::AlignRight);
    m_bottomLeftPlainTextEdit->setAlignment(Qt::AlignLeft);
    m_bottomRightPlainTextEdit->setAlignment(Qt::AlignRight);

    m_modalityLabel->setVisible(modality != "Default");
    m_modalityLineEdit->setVisible(modality != "Default");
    m_modalityLabel->setDisabled(modality == "MG");
    m_modalityLineEdit->setDisabled(modality == "MG");
    m_invertSidesMGCheckBox->setVisible(modality == "MG");

    m_modalityLineEdit->setText(modality);
    Q2DViewerAnnotationsSettingsHelper helper;
    populate(helper.getSettings(modality));

    connect(m_modalityLineEdit, &QLineEdit::editingFinished, [this] {
        if (m_modality == m_modalityLineEdit->text())   // no change
        {
            m_modalityValidated = true;
            return;
        }

        // editingFinished() can be emitted for Enter/Return key press or when the QLineEdit loses focus. If it's emitted for Enter/Return and the change is
        // rejected, then Q2DViewerAnnotationsConfigurationScreen shows an error message and thus QLineEdit loses focus. We need this check to avoid double
        // validation and double error message.
        if (!m_validatingModality)
        {
            m_validatingModality = true;
            emit modalityChanged(m_modalityLineEdit->text());
            m_validatingModality = false;
        }
    });

    connect(m_helpPushButton, &QPushButton::clicked, this, &Q2DViewerAnnotationsConfigurationWidget::helpRequested);

    connect(m_restoreDefaultsPushButton, &QPushButton::clicked, [this] {
        Q2DViewerAnnotationsSettingsHelper helper;
        populate(helper.getDefaultSettings(m_modality));
    });
}

void Q2DViewerAnnotationsConfigurationWidget::editModality()
{
    m_modalityLineEdit->setFocus();
    m_modalityLineEdit->selectAll();
}

void Q2DViewerAnnotationsConfigurationWidget::updateModality()
{
    m_modality = m_modalityLineEdit->text();
    m_modalityValidated = true;
}

void Q2DViewerAnnotationsConfigurationWidget::restoreModality()
{
    m_modalityLineEdit->setText(m_modality);
    m_modalityValidated = false;
}

Q2DViewerAnnotationsSettings Q2DViewerAnnotationsConfigurationWidget::getAnnotationsSettings() const
{
    return {
        m_topLeftPlainTextEdit->toPlainText(),
        m_topRightPlainTextEdit->toPlainText(),
        m_bottomLeftPlainTextEdit->toPlainText(),
        m_bottomRightPlainTextEdit->toPlainText(),
        m_topOrientationCheckBox->isChecked(),
        m_bottomOrientationCheckBox->isChecked(),
        m_leftOrientationCheckBox->isChecked(),
        m_rightOrientationCheckBox->isChecked(),
        m_invertSidesMGCheckBox->isChecked()
    };
}

void Q2DViewerAnnotationsConfigurationWidget::keyPressEvent(QKeyEvent *event)
{
    if ((event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) && !m_modalityValidated)
    {
        return; // eat key press because the change has been rejected
    }
    else if (event->key() == Qt::Key_Escape && m_modalityLineEdit->hasFocus())
    {
        m_modalityLineEdit->clearFocus();   // remove focus from QLineEdit to force emission of its editingFinished() signal, and thus force change validation

        if (!m_modalityValidated)
        {
            return; // eat key press because the change has been rejected
        }
    }

    QWidget::keyPressEvent(event);
}

void Q2DViewerAnnotationsConfigurationWidget::populate(const Q2DViewerAnnotationsSettings &annotationsSettings)
{
    m_topLeftPlainTextEdit->setPlainText(annotationsSettings.topLeft);
    m_topRightPlainTextEdit->setPlainText(annotationsSettings.topRight);
    m_bottomLeftPlainTextEdit->setPlainText(annotationsSettings.bottomLeft);
    m_bottomRightPlainTextEdit->setPlainText(annotationsSettings.bottomRight);
    m_topOrientationCheckBox->setChecked(annotationsSettings.topOrientation);
    m_bottomOrientationCheckBox->setChecked(annotationsSettings.bottomOrientation);
    m_leftOrientationCheckBox->setChecked(annotationsSettings.leftOrientation);
    m_rightOrientationCheckBox->setChecked(annotationsSettings.rightOrientation);
    m_invertSidesMGCheckBox->setChecked(annotationsSettings.invertSidesMG);
}

} // namespace udg
