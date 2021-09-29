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

#include "pacsdevicemodel.h"

#include "pacsdevicemanager.h"

namespace udg {

PacsDeviceModel::PacsDeviceModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

int PacsDeviceModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
    {
        return 0;
    }
    else
    {
        return PacsDeviceManager().getPACSList().size();
    }
}

int PacsDeviceModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
    {
        return 0;
    }
    else
    {
        return NumberOfColumns;
    }
}

QVariant PacsDeviceModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= rowCount() || index.column() < 0 || index.column() >= columnCount())
    {
        return QVariant();
    }

    if (role == Qt::DisplayRole)
    {
        PacsDevice pacsDevice = PacsDeviceManager().getPACSList().at(index.row());

        switch (index.column())
        {
            case PacsId: return pacsDevice.getID();
            case AeTitle: return pacsDevice.getAETitle();
            case Institution: return pacsDevice.getInstitution();
            case Description: return pacsDevice.getDescription();
            case Default: return pacsDevice.isDefault() ? tr("Yes") : tr("No");
        }
    }

    return QVariant();
}

QVariant PacsDeviceModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Vertical || role != Qt::DisplayRole)
    {
        return QVariant();
    }

    switch (section)
    {
        case PacsId: return tr("PacsId");
        case AeTitle: return tr("AE Title");
        case Institution: return tr("Institution");
        case Description: return tr("Description");
        case Default: return tr("Default query PACS");
        default: return QVariant();
    }
}

void PacsDeviceModel::refresh()
{
    beginResetModel();
    endResetModel();
}

} // namespace udg
