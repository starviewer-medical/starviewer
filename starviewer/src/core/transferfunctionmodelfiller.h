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
