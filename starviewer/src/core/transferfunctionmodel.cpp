#include "transferfunctionmodel.h"

#include "transferfunctionio.h"

#include <QDirIterator>

namespace udg {

TransferFunctionModel::TransferFunctionModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

TransferFunctionModel::~TransferFunctionModel()
{
}

int TransferFunctionModel::rowCount(const QModelIndex &) const
{
    return m_transferFunctionList.size();
}

QVariant TransferFunctionModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
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

TransferFunction TransferFunctionModel::getTransferFunction(int index) const
{
    return this->getTransferFunction(this->index(index));
}

TransferFunction TransferFunctionModel::getTransferFunction(const QModelIndex &index) const
{
    if (index.isValid())
    {
        return m_transferFunctionList.at(index.row());
    }
    else
    {
        return TransferFunction();
    }
}

void TransferFunctionModel::loadDefault2DTransferFunctions()
{
    QDirIterator it(":/cluts/2d");

    while (it.hasNext())
    {
        TransferFunction *transferFunction = TransferFunctionIO::fromXmlFile(it.next());
        m_transferFunctionList.append(*transferFunction);
        delete transferFunction;
    }
}

}
