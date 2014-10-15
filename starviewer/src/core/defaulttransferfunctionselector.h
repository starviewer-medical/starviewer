#ifndef DEFAULTTRANSFERFUNCTIONSELECTOR_H
#define DEFAULTTRANSFERFUNCTIONSELECTOR_H

class QString;

namespace udg {

class TransferFunctionModel;

/**
    This class has methods to choose the default transfer function from a given TransferFunctionModel for a specific role.
 */
class DefaultTransferFunctionSelector {

public:
    /// Returns the default transfer function for PET.
    int getDefaultTransferFunctionForPET(const TransferFunctionModel *model) const;

    /// Returns the default transfer function for the PET volume in PET-CT.
    int getDefaultTransferFunctionForPETCT(const TransferFunctionModel *model) const;

    /// Returns the default transfer function for NM.
    int getDefaultTransferFunctionForNM(const TransferFunctionModel *model) const;

private:
    /// Returns the index of the transfer function by name. Returns -1 if not found.
    int getTransferFunctionIndexByName(const QString &name, const TransferFunctionModel *model) const;
};

}

#endif
