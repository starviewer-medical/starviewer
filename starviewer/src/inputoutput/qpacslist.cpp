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

#include "inputoutputsettings.h"
#include "pacsdevice.h"
#include "pacsdevicemodel.h"

#include <QSortFilterProxyModel>

namespace udg {

QPacsList::QPacsList(QWidget *parent)
    : QWidget(parent), m_showQueryPacsDefaultHighlighted(true)
{
    setupUi(this);

    m_pacsDeviceModel = new PacsDeviceModel(m_pacsTableView);
    QSortFilterProxyModel *filterModel = new QSortFilterProxyModel(m_pacsTableView);
    filterModel->setSourceModel(m_pacsDeviceModel);
    m_pacsTableView->setModel(filterModel);

    Settings().restoreColumnsWidths(InputOutputSettings::PacsListColumnWidths, m_pacsTableView);

    // Hide PacsId and Default columns
    m_pacsTableView->setColumnHidden(PacsDeviceModel::PacsId, true);
    m_pacsTableView->setColumnHidden(PacsDeviceModel::Default, true);
    // Order by AE Title / Base URI
    m_pacsTableView->sortByColumn(PacsDeviceModel::AeTitleOrBaseUri, Qt::AscendingOrder);

    connect(m_pacsTableView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &QPacsList::pacsSelectionChanged);

    refresh();
}

QPacsList::~QPacsList()
{
    Settings().saveColumnsWidths(InputOutputSettings::PacsListColumnWidths, m_pacsTableView);
}

QList<PacsDevice> QPacsList::getSelectedPacs() const
{
    QList<PacsDevice> selectedPacsList;
    QModelIndexList selectedRows = m_pacsTableView->selectionModel()->selectedRows(PacsDeviceModel::PacsId);

    for (const QModelIndex &index : selectedRows)
    {
        QString pacsId = m_pacsTableView->model()->data(index).toString();
        selectedPacsList.append(PacsDeviceManager::getPacsDeviceById(pacsId));
    }

    return selectedPacsList;
}

void QPacsList::clearSelection()
{
    m_pacsTableView->clearSelection();
}

void QPacsList::setFilterPACSByService(PacsDeviceManager::PacsFilter filter)
{
    m_pacsDeviceModel->setPacsFilter(filter);
}

void QPacsList::setShowQueryPacsDefaultHighlighted(bool showHighlighted)
{
    m_showQueryPacsDefaultHighlighted = showHighlighted;
}

bool QPacsList::getShowQueryPacsDefaultHighlighted() const
{
    return m_showQueryPacsDefaultHighlighted;
}

void QPacsList::refresh()
{
    m_pacsDeviceModel->refresh();

    if (getShowQueryPacsDefaultHighlighted())
    {
        for (int i = 0; i < m_pacsTableView->model()->rowCount(); i++)
        {
            QModelIndex index = m_pacsTableView->model()->index(i, PacsDeviceModel::Default);

            if (m_pacsTableView->model()->data(index, Qt::UserRole).toBool())
            {
                m_pacsTableView->selectRow(i);
            }
        }
    }
}

}
