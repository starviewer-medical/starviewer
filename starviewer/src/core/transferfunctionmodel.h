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

#ifndef TRANSFERFUNCTIONMODEL_H
#define TRANSFERFUNCTIONMODEL_H

#include <QAbstractListModel>

#include "transferfunction.h"

namespace udg {

/**
    Stores a set of transfer functions and implements the model interface of the Qt model/view framework.
  */
class TransferFunctionModel : public QAbstractListModel {

    Q_OBJECT

public:
    TransferFunctionModel(QObject *parent = 0);
    ~TransferFunctionModel();

    /// Returns the number of rows under the given parent. When the parent is valid it means that rowCount is returning the number of children of parent.
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;

    /// Inserts count rows into the model before the given row. Items in the new row will be children of the item represented by the parent model index.
    virtual bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());

    /// Removes count rows starting with the given row under parent parent from the model.
    virtual bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());

    /// Returns the data stored under the given role for the item referred to by the index.
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    /// Sets the role data for the item at index to value.
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    /// Returns the transfer function at the given index. If the index is out of range, a default transfer function is returned.
    TransferFunction getTransferFunction(int index) const;

    /// Returns the transfer function at the given index. If the index is out of range, a default transfer function is returned.
    TransferFunction getTransferFunction(const QModelIndex &index) const;

    /// Sets the transfer function at the given index. If the index is invalid, it does nothing.
    void setTransferFunction(int index, const TransferFunction &transferFunction);

    /// Sets the transfer function at the given index. If the index is invalid, it does nothing.
    void setTransferFunction(const QModelIndex &index, const TransferFunction &transferFunction);

    /// Returns the index of the given transfer function. If the transfer function is not found, returns -1.
    /// If matchNameOnly is true, then transfer functions are compared only by their name, ignoring their values.
    int getIndexOf(const TransferFunction &transferFunction, bool matchNameOnly = false) const;

private:
    /// List where the transfer functions are stored.
    QList<TransferFunction> m_transferFunctionList;

};

}

#endif
