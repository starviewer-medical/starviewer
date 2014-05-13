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

#include "transferfunctionmodel.h"

namespace udg {

TransferFunctionModel::TransferFunctionModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

TransferFunctionModel::~TransferFunctionModel()
{
}

int TransferFunctionModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
    {
        return 0;
    }

    return m_transferFunctionList.size();
}

bool TransferFunctionModel::insertRows(int row, int count, const QModelIndex &parent)
{
    if (row < 0 || row > rowCount(parent) || count < 1)
    {
        return false;
    }

    beginInsertRows(QModelIndex(), row, row + count - 1);

    for (int i = 0; i < count; i++)
    {
        m_transferFunctionList.insert(row, TransferFunction());
    }

    endInsertRows();

    return true;
}

bool TransferFunctionModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (row < 0 || row + count > rowCount(parent) || count < 1)
    {
        return false;
    }

    beginRemoveRows(QModelIndex(), row, row + count - 1);

    for (int i = 0; i < count; i++)
    {
        m_transferFunctionList.removeAt(row);
    }

    endRemoveRows();

    return true;
}

QVariant TransferFunctionModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= m_transferFunctionList.size())
    {
        return QVariant();
    }

    switch (role)
    {
        case Qt::DisplayRole:
            return m_transferFunctionList.at(index.row()).name();
        case Qt::UserRole:
            return m_transferFunctionList.at(index.row()).toVariant();
        default:
            return QVariant();
    }
}

bool TransferFunctionModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.row() >= 0 && index.row() < m_transferFunctionList.size() && (role == Qt::DisplayRole || role == Qt::UserRole))
    {
        if (role == Qt::DisplayRole)
        {
            m_transferFunctionList[index.row()].setName(value.toString());
        }
        else
        {
            m_transferFunctionList[index.row()] = TransferFunction::fromVariant(value);
        }

        emit dataChanged(index, index);
        return true;
    }

    return false;
}

TransferFunction TransferFunctionModel::getTransferFunction(int index) const
{
    return this->getTransferFunction(this->index(index));
}

TransferFunction TransferFunctionModel::getTransferFunction(const QModelIndex &index) const
{
    if (index.row() >= 0 || index.row() < m_transferFunctionList.size())
    {
        return m_transferFunctionList.at(index.row());
    }
    else
    {
        return TransferFunction();
    }
}

void TransferFunctionModel::setTransferFunction(int index, const TransferFunction &transferFunction)
{
    this->setTransferFunction(this->index(index), transferFunction);
}

void TransferFunctionModel::setTransferFunction(const QModelIndex &index, const TransferFunction &transferFunction)
{
    if (index.row() >= 0 || index.row() < m_transferFunctionList.size())
    {
        m_transferFunctionList[index.row()] = transferFunction;
        emit dataChanged(index, index);
    }
}

int TransferFunctionModel::getIndexOf(const TransferFunction &transferFunction, bool matchNameOnly) const
{
    QModelIndexList matched = match(index(0), Qt::DisplayRole, transferFunction.name(), -1, Qt::MatchFixedString | Qt::MatchCaseSensitive);

    if (matchNameOnly && !matched.isEmpty())
    {
        return matched.first().row();
    }

    foreach (const QModelIndex &index, matched)
    {
        if (getTransferFunction(index) == transferFunction)
        {
            return index.row();
        }
    }

    return -1;
}

}
