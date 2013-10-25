#include "transferfunctionmodelfiller.h"

#include "transferfunctionio.h"
#include "transferfunctionmodel.h"

#include <QDirIterator>

namespace udg {

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
