#include "transferfunctionmodelfiller.h"

#include "transferfunctionio.h"
#include "transferfunctionmodel.h"

#include <QDirIterator>

namespace udg {

void TransferFunctionModelFiller::addEmptyTransferFunction(TransferFunctionModel *model) const
{
    if (model)
    {
        model->insertRow(0);
        model->setData(model->index(0, 0), QObject::tr("None"), Qt::DisplayRole);
    }
}

void TransferFunctionModelFiller::removeEmptyTransferFunction(TransferFunctionModel *model) const
{
    if (model)
    {
        model->removeRow(0);
    }
}

void TransferFunctionModelFiller::add2DTransferFunctions(TransferFunctionModel *model) const
{
    if (model)
    {
        QDirIterator it(":/cluts/2d");

        while (it.hasNext())
        {
            TransferFunction *transferFunction = TransferFunctionIO::fromXmlFile(it.next());
            model->insertRow(model->rowCount());
            model->setTransferFunction(model->rowCount() - 1, *transferFunction);
            delete transferFunction;
        }
    }
}

}
