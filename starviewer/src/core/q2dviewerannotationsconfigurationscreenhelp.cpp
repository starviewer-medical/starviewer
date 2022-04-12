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

#include "q2dviewerannotationsconfigurationscreenhelp.h"

#include <QMouseEvent>

namespace udg {

Q2DViewerAnnotationsConfigurationScreenHelp::Q2DViewerAnnotationsConfigurationScreenHelp(QWidget *parent)
    : QWidget(parent)
{
    setupUi(this);
}

void Q2DViewerAnnotationsConfigurationScreenHelp::addVariable(const QString &variable, const QString &explanation)
{
    int row = m_gridLayout->rowCount();

    QLabel *variableLabel = new QLabel(variable);
    variableLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    variableLabel->installEventFilter(this);
    m_gridLayout->addWidget(variableLabel, row, 0);

    QLabel *explanationLabel = new QLabel(explanation);
    explanationLabel->setWordWrap(true);
    m_gridLayout->addWidget(explanationLabel, row, 1);
}

bool Q2DViewerAnnotationsConfigurationScreenHelp::eventFilter(QObject *watched, QEvent *event)
{
    // Text is selected on left button press and then the event is allowed to continue further processing. This allows the selection to be dragged immediately.
    // On left button release event processing is stopped to avoid deselecting the text.
    QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
    QLabel *label = qobject_cast<QLabel*>(watched);

    if (event->type() == QEvent::MouseButtonPress && mouseEvent->button() == Qt::LeftButton && label)
    {
        label->setSelection(0, label->text().length());
    }
    else if (event->type() == QEvent::MouseButtonRelease && mouseEvent->button() == Qt::LeftButton && label)
    {
        return true;
    }

    return false;
}

} // namespace udg
