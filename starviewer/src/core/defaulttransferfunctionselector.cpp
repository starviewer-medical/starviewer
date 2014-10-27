#include "defaulttransferfunctionselector.h"

#include "logging.h"
#include "transferfunctionmodel.h"

namespace udg {

int DefaultTransferFunctionSelector::getDefaultTransferFunctionForPET(const TransferFunctionModel *model) const
{
    return getTransferFunctionIndexByName("Black & White Inverse", model);
}

int DefaultTransferFunctionSelector::getDefaultTransferFunctionForPETCT(const TransferFunctionModel *model) const
{
    return getTransferFunctionIndexByName("PET Osirix", model);
}

int DefaultTransferFunctionSelector::getDefaultTransferFunctionForNM(const TransferFunctionModel *model) const
{
    return getTransferFunctionIndexByName("Black & White Inverse", model);
}

int DefaultTransferFunctionSelector::getDefaultTransferFunctionForNMCT(const TransferFunctionModel *model) const
{
    return getTransferFunctionIndexByName("PET Osirix", model);
}

int DefaultTransferFunctionSelector::getTransferFunctionIndexByName(const QString &transferFunctionName, const TransferFunctionModel *model) const
{
    // In case of null or empty model, return -1
    if (!model || model->rowCount() == 0)
    {
        return -1;
    }

    // Find the transfer function by name
    TransferFunction transferFunction;
    transferFunction.setName(transferFunctionName);
    int index = model->getIndexOf(transferFunction, true);

    if (index < 0)
    {
        WARN_LOG(QString("Can't find the given transfer function named \"%1\".").arg(transferFunctionName));
    }

    return index;
}

}
