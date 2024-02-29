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

#include "deeplearningtensorflowsegmentation.h"

namespace udg {

DeepLearningTensorFlowSegmentation::DeepLearningTensorFlowSegmentation()
{
    graph = nullptr;
    status = nullptr;
    sessionOpts = nullptr;
    runOpts = nullptr;
    session = nullptr;
    input = nullptr;
    output = nullptr;
    for (int64_t& dim : dims) dim = 0;
    ndata = 0;
    inputValues = nullptr;
    outputValues = nullptr;
}

DeepLearningTensorFlowSegmentation::~DeepLearningTensorFlowSegmentation()
{
    freeMemory();
}

void DeepLearningTensorFlowSegmentation::freeMemory()
{
    if (graph) {
        TF_DeleteGraph(graph);
        TF_DeleteSession(session, status);
        TF_DeleteSessionOptions(sessionOpts);
        TF_DeleteStatus(status);
        free(input);
        free(output);
        free(inputValues);
        free(outputValues);
        // delete input; // use delete if memory allocated by new instead of malloc
        // delete output;
        // delete inputValues;
        // delete outputValues;
    }
}

void DeepLearningTensorFlowSegmentation::fillMask()
{
    if (m_activationFunction == SOFTMAX) {
        softmax();
    }

    // Free memory of input and output tensors
    TF_DeleteTensor(inputValues[0]);
    TF_DeleteTensor(outputValues[0]);
}

void DeepLearningTensorFlowSegmentation::softmax()
{
    // Get mask data (pointer to current mask slice)
    short* maskData = static_cast<short*>(m_outputMask->GetScalarPointer(0, 0, m_currentSlice - m_sliceRange[0]));

    // Get number of pixels of the mask
    int numPixels = m_modelDims[0] * m_modelDims[1];

    // Get data from output value
    void* buff = TF_TensorData(outputValues[0]);
    float* offsets = (float*)buff;

    // The model returns a tensor with the following shape:
    // (batch_size, height, width, channels)
    // When running inference, the label assigned to the pixel is the
    // channel with the highest value.
    // There are 2 channels per pixel:
    //  - 1st channel: probability for the mask's pixel to be 0
    //  - 2nd channel: probability for the mask's pixel to be 1
    // Each pixel of the mask will take 0 or 1 depending on which is the
    // largest probability. That's what the loop does: if the 1st channel
    // has the largest value, pixel takes 0; if the 2nd channel has the
    // largest value, pixel takes 1.

    // Create the mask from the offsets (values of the two channels)
    for (int i = 0; i < numPixels; i++) {
        // Assign index of the channel (0/1) with highest value
        *maskData++ = offsets[i*2] > offsets[i*2+1] ? 0 : 1;
    }
}

void DeepLearningTensorFlowSegmentation::loadModelParameters()
{
    // Load TensorFlow model using TF API C


    // **
    // * Read model
    // **

    // Free memory if a previous model is loaded
    if (graph) {
        freeMemory();
    }

    graph = TF_NewGraph();
    status = TF_NewStatus();
    sessionOpts = TF_NewSessionOptions();
    runOpts = NULL;

    // When we save a model (TF), a folder will be created with a bunch of
    // files inside it. It basically stores the weights and the graphs of
    // the model. TensorFlow has a command-line tool to dive into these
    // files for us to match the input tensor and the output tensor. It is
    // called `saved_model_cli`, and comes together with the tools provided
    // when installing TensorFlow.
    // To run it in Windows (in the corresponding directory):
    // $ py .\saved_model_cli.py

    // We need to extract the graph name for the input tensor and output
    // tensor and use this information when calling C API. Run:
    // $ saved_model_cli show --dir <path_to_saved_model_folder>
    // The following output (or similar) is obtained:
    // > The given SavedModel contains the following tag-sets:
    // > 'serve'
    const char* tags = "serve";
    int ntags = 1;

    // Read saved model
    session = TF_LoadSessionFromSavedModel(
                sessionOpts,
                runOpts,
                qPrintable(m_modelPath),
                &tags,
                ntags,
                graph,
                NULL,
                status
    );

    if (TF_GetCode(status) != TF_OK) {
        throw std::exception(TF_Message(status));
    }


    // **
    // * Get input and output tensors
    // **

    // Get the input and output tensors from the graph by their name.
    // Use the previous "tag-set" (`serve`) to further drill into the
    // tensor graph. Run:
    // $ saved_model_cli show --dir <path_to_saved_model_folder>
    //   --tag_set serve
    // The following output (or similar) is obtained:
    // > The given SavedModel MetaGraphDef contains SignatureDefs with the
    //   following keys:
    // > SignatureDef key: "__saved_model_init_op"
    // > SignatureDef key: "serving_default"
    //
    // Use the `serving_default` signature key into the command to print
    // out the tensor node. Run:
    // $ saved_model_cli show --dir <path_to_saved_model_folder>
    //   --tag-set serve
    //   --signature_def serving_default
    // The following output (or similar) is obtained:
    // > The given SavedModel SignatureDef contains the following input(s):
    // >   inputs['input_2'] tensor_info:
    // >       dtype: DT_FLOAT
    // >       shape: (-1, 128, 128, 3)
    // >       name: serving_default_input_2:0
    // > The given SavedModel SignatureDef contains the following
    //   output(s):
    // >   outputs['conv2d_transpose_4'] tensor_info:
    // >       dtype: DT_FLOAT
    // >       shape: (-1, 128, 128, 2)
    // >       name: StatefulPartitionedCall:0
    // > Method name is: tensorflow/serving/predict
    //
    // Input tensor name is: `serving_default_input_2`
    // Output tensor name is: `StatefulPartitionedCall`

    // Local full command:
    // py .\saved_model_cli.py show --dir path\to\model.tf --tag_set serve --signature_def serving_default

    // Get input tensor
    input = (TF_Output*)malloc(sizeof(TF_Output) * numInputs);
    // input = new TF_Output[numInputs]; // equivalent?
    TF_Output t0 = {TF_GraphOperationByName(graph, "serving_default_input_2"), 0};

    if (t0.oper == NULL) {
        throw std::exception("ERROR: Failed TF_GraphOperationByName serving_default_input_2");
    }

    input[0] = t0;

    // Get output tensor
    output = (TF_Output*)malloc(sizeof(TF_Output) * numOutputs);
    TF_Output t2 = {TF_GraphOperationByName(graph, "StatefulPartitionedCall"), 0};

    if (t2.oper == NULL) {
        throw std::exception("ERROR: Failed TF_GraphOperationByName StatefulPartitionedCall");
    }

    output[0] = t2;


    // **
    // * Allocate data for inputs and outputs
    // **

    inputValues = (TF_Tensor**)malloc(sizeof(TF_Tensor*) * numInputs);
    outputValues = (TF_Tensor**)malloc(sizeof(TF_Tensor*) * numOutputs);

    dims[0] = 1;
    dims[1] = m_modelDims[0];
    dims[2] = m_modelDims[1];
    dims[3] = m_numChannels;

    // Get number of pixels for output image
    int numPixels = m_modelDims[0] * m_modelDims[1];

    // `ndata` is total byte size of the data, not length of the array
    ndata = sizeof(float) * numPixels * m_numChannels;
}

void DeepLearningTensorFlowSegmentation::run(float* data)
{
    // Allocate the new tensor locally using `TF_NewTensor`
    TF_Tensor* in_tensor = TF_NewTensor(
                TF_FLOAT,
                dims,
                ndims,
                data,
                ndata,
                &DeepLearningTensorFlowSegmentation::NoOpDeallocator,
                0
    );

    if (in_tensor == NULL) {
        throw std::exception("ERROR: Failed TF_NewTensor");
    }

    // Set the input value
    inputValues[0] = in_tensor;

    // Run the session by invoking `TF_SessionRun`
    TF_SessionRun(
                session,
                NULL,
                input,
                inputValues,
                numInputs,
                output,
                outputValues,
                numOutputs,
                NULL,
                0,
                NULL,
                status
    );

    if (TF_GetCode(status) != TF_OK) {
        throw std::exception(TF_Message(status));
    }
}

} // namespace udg
