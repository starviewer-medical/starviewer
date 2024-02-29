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

#ifndef UDG_DEEPLEARNINGTENSORFLOWSEGMENTATION_H
#define UDG_DEEPLEARNINGTENSORFLOWSEGMENTATION_H

#include <tensorflow/c/c_api.h>

#include "deeplearningsegmentation.h"

namespace udg {

/**
 * @brief The DeepLearningTensorFlowSegmentation class encapsulates the methods
 * to perform a segmentation using a TensorFlow model.
 */
class DeepLearningTensorFlowSegmentation : public DeepLearningSegmentation
{
public:

    DeepLearningTensorFlowSegmentation();
    ~DeepLearningTensorFlowSegmentation();

private:

    /// TensorFlow graph.
    TF_Graph* graph;
    /// Status of a TensorFlow operation.
    TF_Status* status;
    /// TensorFlow session options.
    TF_SessionOptions* sessionOpts;
    /// TensorFlow run options.
    TF_Buffer* runOpts;
    /// TensorFlow session (used to run the model).
    TF_Session* session;
    /// Number of model inputs (currently restricted to 1).
    constexpr static int numInputs = 1;
    /// Input tensor.
    TF_Output* input;
    /// Number of model outputs (currently restricted to 1).
    constexpr static int numOutputs = 1; // var
    /// Output tensor.
    TF_Output* output;
    /// Number of input dimensions (batch size, height, width, channels).
    constexpr static int ndims = 4;
    /// Input dimensions (batch size, height, width, channels).
    int64_t dims[ndims];
    /// Total byte size of the input data (not length of the array).
    int ndata;
    /// Array of input tensors.
    TF_Tensor** inputValues;
    /// Array of output tensors.
    TF_Tensor** outputValues;

    /// Free memory allocated when loading the TensorFlow model.
    void freeMemory();

    /// Custom deallocator function needed for `TF_NewTensor` to deallocate
    /// data so that custom-managed memory can be passed in (does nothing).
    /// Must be static to be able to invoke it without an object.
    static void NoOpDeallocator(void* data, size_t a, void* b) {}

protected:

    /// Load a TensorFlow deep-learning model.
    void loadModelParameters();

    /// Run the TensorFlow model using input data and store the result.
    void run(float* data);

    /// Interpret the result stored depending on the activation function and
    /// fill the mask accordingly.
    void fillMask();

    /// Interpret the result stored using the Softmax activation function and
    /// fill the mask accordingly.
    void softmax();
};

} // namespace udg

#endif // UDG_DEEPLEARNINGTENSORFLOWSEGMENTATION_H
