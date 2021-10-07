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

#include "qpacslist.h"

#include <QTreeView>
#include <QList>

#include "pacsdevicemanager.h"
#include "pacsdevice.h"

namespace udg {

QPacsList::QPacsList(QWidget *parent)
 : QWidget(parent)
{
    setupUi(this);

    // La columna PacsId està amagada
    m_PacsTreeView->setColumnHidden(0, true);
    // La columna PACSAddress està amagada
    m_PacsTreeView->setColumnHidden(4, true);
    // Ordenem per la columna AETitle
    m_PacsTreeView->sortByColumn(1, Qt::AscendingOrder);

    m_pacsFilter = PacsDeviceManager::AllTypes;
    m_showQueryPacsDefaultHighlighted = true;

    connect(m_PacsTreeView, SIGNAL(itemSelectionChanged()), SIGNAL(pacsSelectionChanged()));

    refresh();
}

QPacsList::~QPacsList()
{
}

void QPacsList::refresh()
{
    QList<PacsDevice> pacsList;

    m_PacsTreeView->clear();

    pacsList = PacsDeviceManager::getPacsList(m_pacsFilter);

    foreach (PacsDevice pacs, pacsList)
    {
        QTreeWidgetItem *item = new QTreeWidgetItem(m_PacsTreeView);

        item->setText(0, pacs.getID());
        item->setText(1, pacs.getAETitle());
        item->setText(2, pacs.getInstitution());
        item->setText(3, pacs.getDescription());
        item->setText(4, pacs.getAddress());

        if (getShowQueryPacsDefaultHighlighted())
        {
            item->setSelected(pacs.isDefault());
        }
    }
}

QList<PacsDevice> QPacsList::getSelectedPacs()
{
    QList<PacsDevice> selectedPacsList;
    QList<QTreeWidgetItem*> qPacsList(m_PacsTreeView->selectedItems());

    QTreeWidgetItem *item;
    for (int i = 0; i < qPacsList.count(); i++)
    {
        PacsDevice pacs;
        item = qPacsList.at(i);
        // Fem el query per cercar la informació del PACS
        pacs = PacsDeviceManager::getPacsDeviceById(item->text(0));
        // Inserim a la llista
        selectedPacsList.append(pacs);
    }

    return selectedPacsList;
}

void QPacsList::clearSelection()
{
    m_PacsTreeView->clearSelection();
}

void QPacsList::setFilterPACSByService(PacsDeviceManager::PacsFilter filter)
{
    m_pacsFilter = filter;
}

PacsDeviceManager::PacsFilter QPacsList::getFilterPACSByService()
{
    return m_pacsFilter;
}

void QPacsList::setShowQueryPacsDefaultHighlighted(bool showHighlighted)
{
    m_showQueryPacsDefaultHighlighted = showHighlighted;
}

bool QPacsList::getShowQueryPacsDefaultHighlighted()
{
    return m_showQueryPacsDefaultHighlighted;
}

void QPacsList::setDefaultPACS(QTreeWidgetItem *item)
{
    Q_ASSERT(item);

    PacsDevice pacs = PacsDeviceManager::getPacsDeviceById(item->text(0));
    pacs.setDefault(item->isSelected());
}

};
