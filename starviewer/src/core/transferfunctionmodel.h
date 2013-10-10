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

    /// Returns the data stored under the given role for the item referred to by the index.
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    /// Returns the transfer function at the given index. If the index is out of range, a default transfer function is returned.
    TransferFunction getTransferFunction(int index) const;

    /// Returns the transfer function at the given index. If the index is out of range, a default transfer function is returned.
    TransferFunction getTransferFunction(const QModelIndex &index) const;

    /// Loads the default 2D transfer functions from resources.
    void loadDefault2DTransferFunctions();

private:
    /// List where the transfer functions are stored.
    QList<TransferFunction> m_transferFunctionList;

};

}

#endif
