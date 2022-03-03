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

#include "pacsdevice.h"

namespace udg {

PacsDeviceModel::PacsDeviceModel(QObject *parent)
    : QAbstractTableModel(parent), m_pacsFilter(PacsDeviceManager::AllTypes)
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
        return PacsDeviceManager::getPacsList(m_pacsFilter).size();
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

    if (role == Qt::DisplayRole || role == Qt::UserRole)
    {
        PacsDevice pacsDevice = PacsDeviceManager::getPacsList(m_pacsFilter).at(index.row());

        switch (index.column())
        {
            case PacsId: return pacsDevice.getID();
            case AeTitleOrBaseUri: return pacsDevice.getType() == PacsDevice::Type::Dimse ? pacsDevice.getAETitle() : pacsDevice.getBaseUri().toString();
            case Institution: return pacsDevice.getInstitution();
            case Description: return pacsDevice.getDescription();
            case Default:
                if (role == Qt::DisplayRole)
                {
                    return pacsDevice.isDefault() ? tr("Yes") : tr("No");
                }
                else    // role == Qt::UserRole
                {
                    return pacsDevice.isDefault();
                }
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
        case AeTitleOrBaseUri: return tr("AE Title / Base URI");
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

void PacsDeviceModel::setPacsFilter(PacsDeviceManager::PacsFilter filter)
{
    m_pacsFilter = filter;
}

} // namespace udg
