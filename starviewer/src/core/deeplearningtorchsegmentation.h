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

#ifndef UDG_DEEPLEARNINGTORCHSEGMENTATION_H
#define UDG_DEEPLEARNINGTORCHSEGMENTATION_H

// Torch library has a function named `slots` which rises a conflict with the
// global scope-defined Qt slots. Qt slots must be undefined before including
// the Torch library, and then defined again.
#undef slots
#include <torch/script.h>
#define slots Q_SLOTS

#include "deeplearningsegmentation.h"

namespace udg {

/**
 * @brief The DeepLearningTorchSegmentation class encapsulates the methods to
 * perform a segmentation using a Torch model.
 */
class DeepLearningTorchSegmentation : public DeepLearningSegmentation
{
public:

    DeepLearningTorchSegmentation();
    ~DeepLearningTorchSegmentation() = default;

private:

    /// Torch module.
    torch::jit::script::Module module;
    /// Output tensor.
    at::Tensor output;

protected:

    /// Load a Torch deep-learning model.
    void loadModelParameters();

    /// Run the Torch model using input data and store the result.
    void run(float* data);

    /// Interpret the result stored depending on the activation function and
    /// fill the mask accordingly.
    void fillMask();

    /// Interpret the result stored using the Sigmoid activation function and
    /// fill the mask accordingly.
    void sigmoid();
};

} // namespace udg

#endif // UDG_DEEPLEARNINGTORCHSEGMENTATION_H
