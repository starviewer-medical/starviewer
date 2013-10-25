#ifndef TRANSFERFUNCTIONMODELFILLER_H
#define TRANSFERFUNCTIONMODELFILLER_H

namespace udg {

class TransferFunctionModel;

/**
    This class has methods to fill a given TransferFunctionModel with specific sets of transfer functions.
  */
class TransferFunctionModelFiller {

public:
    /// Adds the 2D transfer functions from resources to the given model.
    void add2DTransferFunctions(TransferFunctionModel *model) const;

};

}

#endif
