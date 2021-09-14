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

#include "qdiagnosistestsresultsitem.h"

#include "diagnosistest.h"

#include <QLabel>

namespace udg {

QDiagnosisTestsResultsItem::QDiagnosisTestsResultsItem(const DiagnosisTest &test, const DiagnosisTestResult &result, QWidget *parent)
    : QWidget(parent)
{
    init();

    QPixmap successIcon(":/images/icons/emblem-success.svg");
    QPixmap warningIcon(":/images/icons/emblem-warning.svg");
    QPixmap errorIcon(":/images/icons/emblem-error.svg");

    switch (result.getState())
    {
        case DiagnosisTestResult::Ok:
            m_titlePushButton->setIcon(successIcon);
            break;
        case DiagnosisTestResult::Warning:
            m_titlePushButton->setIcon(warningIcon);
            break;
        case DiagnosisTestResult::Error:
            m_titlePushButton->setIcon(errorIcon);
            break;
    }

    m_titlePushButton->setText(test.getDescription());

    if (result.getState() == DiagnosisTestResult::Ok)
    {
        m_titlePushButton->setCheckable(false);
        m_titlePushButton->unsetCursor();
    }
    else
    {
        int i = 0;

        foreach (const DiagnosisTestProblem &problem, result.getErrors() + result.getWarnings())
        {
            QLabel *iconLabel = new QLabel();
            iconLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
            iconLabel->setAlignment(Qt::AlignTop);
            switch (problem.getState())
            {
                case DiagnosisTestProblem::Warning:
                    iconLabel->setPixmap(warningIcon);
                    break;
                case DiagnosisTestProblem::Error:
                    iconLabel->setPixmap(errorIcon);
                    break;
                default:
                    break;
            }
            m_contentGridLayout->addWidget(iconLabel, i, 0);

            QLabel *descriptionLabel = new QLabel(QString("<b>%1</b>").arg(problem.getDescription()));
            descriptionLabel->setWordWrap(true);
            descriptionLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
            m_contentGridLayout->addWidget(descriptionLabel, i++, 1);

            if (!problem.getSolution().isEmpty())
            {
                QLabel *solutionLabel = new QLabel(problem.getSolution());
                solutionLabel->setWordWrap(true);
                solutionLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
                m_contentGridLayout->addWidget(solutionLabel, i++, 1);
            }
        }
    }
}

QDiagnosisTestsResultsItem::QDiagnosisTestsResultsItem(const QString &title, const QStringList &content, QWidget *parent)
    : QWidget(parent)
{
    init();

    QPixmap icon(":/images/icons/emblem-information.svg");
    m_titlePushButton->setIcon(icon);
    m_titlePushButton->setText(title);

    if (content.isEmpty())
    {
        m_titlePushButton->setCheckable(false);
    }
    else
    {
        m_contentGridLayout->setColumnMinimumWidth(0, 16 + m_contentGridLayout->spacing());
        QLabel *label = new QLabel(content.join("<p>"));
        label->setWordWrap(true);
        label->setTextInteractionFlags(Qt::TextSelectableByMouse);
        m_contentGridLayout->addWidget(label, 0, 1);
    }
}

void QDiagnosisTestsResultsItem::init()
{
    setupUi(this);

    m_contentFrame->hide();

    connect(m_titlePushButton, &QPushButton::toggled, m_contentFrame, &QFrame::setVisible);
}

} // namespace udg
