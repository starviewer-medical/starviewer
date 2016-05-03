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

#include "externalapplicationsmanager.h"

#include "coresettings.h"
#include "externalapplication.h"
#include "logging.h"
#include "qexternalapplicationconfigurationscreen.h"

#include <QCloseEvent>

namespace udg {

QExternalApplicationConfigurationScreen::QExternalApplicationConfigurationScreen(QWidget *parent)
 : QWidget(parent)
{
    setupUi(this);
    checkGrayeds();
    this->setExternalApplications(ExternalApplicationsManager::instance()->getApplications());

    connect(m_buttonAddCommand, SIGNAL(clicked()), this, SLOT(addCommandClicked()));
    connect(m_buttonAddUrl, SIGNAL(clicked()), this, SLOT(buttonAddUrlClicked()));
    connect(m_buttonDelete, SIGNAL(clicked()), this, SLOT(buttonDeleteClicked()));
    connect(m_buttonDown, SIGNAL(clicked()), this, SLOT(buttonDownClicked()));
    connect(m_buttonUp, SIGNAL(clicked()), this, SLOT(buttonUpClicked()));
    connect(m_tableWidget, SIGNAL(currentItemChanged(QTableWidgetItem*, QTableWidgetItem*)), this, SLOT(tableCurrentItemChanged(QTableWidgetItem*, QTableWidgetItem*)));
}

QExternalApplicationConfigurationScreen::~QExternalApplicationConfigurationScreen()
{

}

void QExternalApplicationConfigurationScreen::setExternalApplications(const QList<ExternalApplication> &externalApplications)
{
    QList<ExternalApplication>::const_iterator it;
    for (it = externalApplications.constBegin(); it != externalApplications.constEnd(); ++it)
    {
        this->addApplication(*it);
    }
}

QList<ExternalApplication> QExternalApplicationConfigurationScreen::getExternalApplications() const
{
    QList<ExternalApplication> applications;
    for (int i = 0; i < m_tableWidget->rowCount(); i++)
    {
        ExternalApplication::ExternalApplicationType type =
                m_tableWidget->item(i,0)->text() == tr("URL") ?
                ExternalApplication::ExternalApplicationType::Url :
                ExternalApplication::ExternalApplicationType::Command;
        QString name = m_tableWidget->item(i, 1)->text();
        QString url = m_tableWidget->item(i, 2)->text();
        applications.append(ExternalApplication(name, url, type));
    }
    return applications;
}

void QExternalApplicationConfigurationScreen::addApplication(const ExternalApplication &externalApplication)
{
    QTableWidgetItem* typeWidget = new QTableWidgetItem();
    QTableWidgetItem* nameWidget = new QTableWidgetItem(externalApplication.getName());
    QTableWidgetItem* urlWidget = new QTableWidgetItem(externalApplication.getUrl());
    typeWidget->setFlags(Qt::ItemIsSelectable);

    if (externalApplication.getType() == ExternalApplication::ExternalApplicationType::Command)
    {
        typeWidget->setText(tr("Command"));
    }
    else
    {
        typeWidget->setText(tr("URL"));
    }

    m_tableWidget->insertRow(m_tableWidget->rowCount());
    m_tableWidget->setItem(m_tableWidget->rowCount()-1, 0, typeWidget);
    m_tableWidget->setItem(m_tableWidget->rowCount()-1, 1, nameWidget);
    m_tableWidget->setItem(m_tableWidget->rowCount()-1, 2, urlWidget);

    checkGrayeds();
}

void QExternalApplicationConfigurationScreen::moveItem(int shift)
{
    //This wants to be understandable code:
    //Do not blame me for efficiency, compiler will make it efficient.
    int currentRow = m_tableWidget->currentRow();
    int currentCol = m_tableWidget->currentColumn();
    if (currentRow >= 0)
    {
        int maxRow = m_tableWidget->rowCount() - 1;
        int minRow = 0;
        int swappingRow = qMin(maxRow,qMax(currentRow + shift, minRow));

        QTableWidgetItem* currentRowCol0 = m_tableWidget->takeItem(currentRow, 0);
        QTableWidgetItem* currentRowCol1 = m_tableWidget->takeItem(currentRow, 1);
        QTableWidgetItem* currentRowCol2 = m_tableWidget->takeItem(currentRow, 2);
        QTableWidgetItem* swappingRowCol0 = m_tableWidget->takeItem(swappingRow, 0);
        QTableWidgetItem* swappingRowCol1 = m_tableWidget->takeItem(swappingRow, 1);
        QTableWidgetItem* swappingRowCol2 = m_tableWidget->takeItem(swappingRow, 2);

        m_tableWidget->setItem(currentRow, 0, swappingRowCol0);
        m_tableWidget->setItem(currentRow, 1, swappingRowCol1);
        m_tableWidget->setItem(currentRow, 2, swappingRowCol2);
        m_tableWidget->setItem(swappingRow, 0, currentRowCol0);
        m_tableWidget->setItem(swappingRow, 1, currentRowCol1);
        m_tableWidget->setItem(swappingRow, 2, currentRowCol2);

        m_tableWidget->setCurrentItem(m_tableWidget->item(swappingRow, currentCol));
    }
}

void QExternalApplicationConfigurationScreen::checkGrayeds()
{
    int maxRow = m_tableWidget->rowCount() - 1;
    int currentRow = m_tableWidget->currentRow();

    m_buttonDelete->setEnabled(currentRow >= 0);
    m_buttonUp->setEnabled(currentRow > 0 && currentRow <= maxRow);
    m_buttonDown->setEnabled(currentRow >= 0 && currentRow < maxRow);
}

void QExternalApplicationConfigurationScreen::buttonUpClicked()
{
    moveItem(-1);
}

void QExternalApplicationConfigurationScreen::buttonDownClicked()
{
    moveItem(1);
}

void QExternalApplicationConfigurationScreen::buttonAddUrlClicked()
{
    ExternalApplication newApp(tr("New application"), "http://www.starviewer.org", ExternalApplication::ExternalApplicationType::Url);
    this->addApplication(newApp);
}

void QExternalApplicationConfigurationScreen::addCommandClicked()
{
    ExternalApplication newApp(tr("New application"), "echo \"Starviewer Medical {%AccessionNumber%}\"", ExternalApplication::ExternalApplicationType::Command);
    this->addApplication(newApp);
}

void QExternalApplicationConfigurationScreen::buttonDeleteClicked()
{
    int rowToRemove = m_tableWidget->currentRow();
    if (rowToRemove >= 0)
    {
        m_tableWidget->removeRow(rowToRemove);
        checkGrayeds();
    }
}

void QExternalApplicationConfigurationScreen::tableCurrentItemChanged(QTableWidgetItem* current, QTableWidgetItem* previous)
{
     checkGrayeds();
}

void QExternalApplicationConfigurationScreen::closeEvent(QCloseEvent *event)
{
    ExternalApplicationsManager::instance()->setApplications(this->getExternalApplications());
    event->accept();
}

}
