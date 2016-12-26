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

#include "qviewerworkinprogresswidget.h"

#include <QMovie>

namespace udg {

QViewerWorkInProgressWidget::QViewerWorkInProgressWidget(QWidget *parent)
 : QWidget(parent)
{
    this->setupUi(this);

    m_progressBarAnimation = new QMovie(this);
    m_progressBarAnimation->setFileName(":/images/animations/downloading.gif");
    m_progressBarLabel->setMovie(m_progressBarAnimation);


    this->reset();
}

void QViewerWorkInProgressWidget::reset()
{
    this->resetProgressWidgets();
    m_headerLabel->setText("");
    m_errorLabel->setText("");
}

void QViewerWorkInProgressWidget::setTitle(const QString &text)
{
    m_headerLabel->setText(text);
}

void QViewerWorkInProgressWidget::showError(const QString &errorText)
{
    this->resetProgressWidgets();
    m_progressBarLabel->hide();
    m_errorLabel->setText(errorText);
}

void QViewerWorkInProgressWidget::updateProgress(int progress)
{
    m_progressLabel->setText(tr(" (%1\%)").arg(progress));
}

void QViewerWorkInProgressWidget::showEvent(QShowEvent *event)
{
    Q_UNUSED(event);
    m_progressBarAnimation->start();
}

void QViewerWorkInProgressWidget::hideEvent(QHideEvent *event)
{
    Q_UNUSED(event);
    m_progressBarAnimation->setPaused(true);
}

void QViewerWorkInProgressWidget::resetProgressWidgets()
{
    m_progressBarLabel->show();
    m_progressLabel->setText("");
}

} // End namespace udg
