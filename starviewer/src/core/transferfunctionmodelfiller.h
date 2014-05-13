#ifndef TRANSFERFUNCTIONMODELFILLER_H
#define TRANSFERFUNCTIONMODELFILLER_H

namespace udg {

class TransferFunctionModel;

/**
    This class has methods to fill a given TransferFunctionModel with specific sets of transfer functions.
  */
class TransferFunctionModelFiller {

public:
    /// Adds a special empty transfer function to the given model.
    void addEmptyTransferFunction(TransferFunctionModel *model) const;

    /// Removes the special empty transfer function from the given model.
    void removeEmptyTransferFunction(TransferFunctionModel *model) const;

    /// Adds the 2D transfer functions from resources to the given model.
    void add2DTransferFunctions(TransferFunctionModel *model) const;

};

}

#endif
