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

#include "logging.h"
#include "qexternalapplicationconfigurationscreen.h"

namespace udg {

QExternalApplicationConfigurationScreen::QExternalApplicationConfigurationScreen(QWidget *parent)
 : QWidget(parent)
{
    setupUi(this);
    checkGrayeds();
}

QExternalApplicationConfigurationScreen::~QExternalApplicationConfigurationScreen()
{

}

void QExternalApplicationConfigurationScreen::setExternalApplications(const QList<ExternalApplication> &externalApplications)
{
    QList<ExternalApplication>::const_iterator it;
    for (it = externalApplications.constBegin(); it != externalApplications.constEnd(); ++it) {
        this->addApplication(*it);
    }
}

QList<ExternalApplication> QExternalApplicationConfigurationScreen::getExternalApplications() const
{
    QList<ExternalApplication> applications;
    for (int i = 0; i < tableWidget->rowCount(); i++) {
        QString name = tableWidget->item(i,0)->text();
        QString url = tableWidget->item(i,1)->text();
        applications.append(ExternalApplication(name,url));
    }
    return applications;
}

void QExternalApplicationConfigurationScreen::addApplication(const ExternalApplication &externalApplication)
{
    QTableWidgetItem* nameWidget = new QTableWidgetItem(externalApplication.getName());
    QTableWidgetItem* urlWidget = new QTableWidgetItem(externalApplication.getUrl());
    tableWidget->insertRow(tableWidget->rowCount());
    tableWidget->setItem(tableWidget->rowCount()-1,0,nameWidget);
    tableWidget->setItem(tableWidget->rowCount()-1,1,urlWidget);
    checkGrayeds();
}

void QExternalApplicationConfigurationScreen::moveItem(int shift)
{
    //This wants to be understandable code:
    //Do not blame me for efficiency, compiler will make it efficient.
    int currentRow = tableWidget->currentRow();
    int currentCol = tableWidget->currentColumn();
    if (currentRow >= 0) {
        int maxRow = tableWidget->rowCount()-1;
        int minRow = 0;
        int swappingRow = qMin(maxRow,qMax(currentRow + shift,minRow));

        QTableWidgetItem* currentRowCol0 = tableWidget->takeItem(currentRow,0);
        QTableWidgetItem* currentRowCol1 = tableWidget->takeItem(currentRow,1);
        QTableWidgetItem* swappingRowCol0 = tableWidget->takeItem(swappingRow,0);
        QTableWidgetItem* swappingRowCol1 = tableWidget->takeItem(swappingRow,1);

        tableWidget->setItem(currentRow,0,swappingRowCol0);
        tableWidget->setItem(currentRow,1,swappingRowCol1);
        tableWidget->setItem(swappingRow,0,currentRowCol0);
        tableWidget->setItem(swappingRow,1,currentRowCol1);

        tableWidget->setCurrentItem(tableWidget->item(swappingRow,currentCol));
    }
}

void QExternalApplicationConfigurationScreen::checkGrayeds()
{
    int maxRow = tableWidget->rowCount() - 1;
    int currentRow = tableWidget->currentRow();

    btnDelete->setEnabled(currentRow >= 0);
    btnUp->setEnabled(currentRow > 0 && currentRow <= maxRow);
    btnDown->setEnabled(currentRow >= 0 && currentRow < maxRow);
}

void QExternalApplicationConfigurationScreen::on_btnUp_clicked() 
{
    moveItem(-1);
}

void QExternalApplicationConfigurationScreen::on_btnDown_clicked() 
{
    moveItem(1);
}

void QExternalApplicationConfigurationScreen::on_btnAdd_clicked() 
{
    ExternalApplication newApp("New application", "http://www.starviewer.org");
    this->addApplication(newApp);
}

void QExternalApplicationConfigurationScreen::on_btnDelete_clicked() 
{
    int rowToRemove = tableWidget->currentRow();
    if (rowToRemove >= 0) {
        tableWidget->removeRow(rowToRemove);
        checkGrayeds();
    }
}

void QExternalApplicationConfigurationScreen::on_tableWidget_currentItemChanged(QTableWidgetItem* current, QTableWidgetItem* previous)
{
     checkGrayeds();
}

}
