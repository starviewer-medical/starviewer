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

#include "deeplearningtorchsegmentation.h"

namespace udg {

DeepLearningTorchSegmentation::DeepLearningTorchSegmentation()
{}

void DeepLearningTorchSegmentation::fillMask()
{
    if (m_activationFunction == SIGMOID) {
        sigmoid();
    }
}

void DeepLearningTorchSegmentation::sigmoid()
{
    // Interpret the output (sigmoid) and get probabilities
    output = torch::sigmoid(output);
    float* probs = (float*)output.data_ptr();

    // Get mask data (pointer to current mask slice)
    short* maskData = static_cast<short*>(m_outputMask->GetScalarPointer(0, 0, m_currentSlice - m_sliceRange[0]));

    // Get number of pixels of the mask
    int numPixels = m_modelDims[0] * m_modelDims[1];

    // Fill the mask from the probabilities
    for (int i = 0; i < numPixels; i++) {
        // Is part of the mask if prob >= 0.5
        *maskData++ = probs[i] >= 0.5 ? 1 : 0;
    }
}

void DeepLearningTorchSegmentation::loadModelParameters()
{
    // Load Torch model using libtorch library
    module = torch::jit::load(m_modelPath.toStdString());
    module.eval();
}

void DeepLearningTorchSegmentation::run(float *data)
{
    // Structure the data in 4 dimensions (batch, width, height, channels),
    // bring the channels' dimension to the second position, and prepare
    // the input
    // (channels are contiguous in memory, so they must be read at the end
    // and then changed with the "permute" operation)
    std::vector<torch::jit::IValue> inputs;
    at::Tensor input = torch::from_blob(data, {1, m_modelDims[0], m_modelDims[1], m_numChannels});
    inputs.push_back(input.permute({0, 3, 1, 2}));

    // Run the model and get the output
    output = module.forward(inputs).toTensor();
}

} // namespace udg
