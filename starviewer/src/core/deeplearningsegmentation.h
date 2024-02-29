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

#ifndef UDG_DEEPLEARNINGSEGMENTATION_H
#define UDG_DEEPLEARNINGSEGMENTATION_H

#include <vtkImageData.h>
#include <vtkSmartPointer.h>
#include <vtkImageReslice.h>

#include <QVector>
#include <QString>

namespace udg {

/**
 * @brief The DeepLearningSegmentation class encapsulates the methods to perform
 * a segmentation using a deep-learning model.
 */
class DeepLearningSegmentation
{
public:
    enum ActivationFunction
    {
        SIGMOID,
        SOFTMAX
    };

    DeepLearningSegmentation();
    virtual ~DeepLearningSegmentation(){};

    /// Get input dimension indices corresponding to the 3D axes.
    QVector<int> getAxes() const;

    /// Set input image on which the segmentation will take place.
    void setInput(vtkImageData* image);

    /// Specify input dimension indices corresponding to the 3D axes.
    void setAxes(int x, int y, int z);
    /// @copydoc setAxes(int,int,int)
    void setAxes(const int axes[3]);

    /// Specify the direction cosines for the input reslice axes.
    void setCosines(double x0, double x1, double x2,
                    double y0, double y1, double y2,
                    double z0, double z1, double z2);
    /// @copydoc setCosines(double,double,double,double,double,double,double,double,double)
    void setCosines(const double cosines[9]);

    /// Specify input dimensions expected by the model.
    void setModelInputDimensions(int x, int y, int z);
    /// @copydoc setModelInputDimensions(int,int,int)
    void setModelInputDimensions(const int dims[3]);

    /// Specify whether input image should be normalised.
    void setNormalisation(bool isNormalised);

    /// Specify the number of channels expected by the model.
    void setNumberOfChannels(int channels);

    /// Set number of labels to be predicted (> 1).
    void setNumberOfLabels(int labels);

    /// Set activation function for probabilities.
    void setActivationFunction(ActivationFunction function);

    /// Set slice range to be segmented (both ends included).
    void setSliceRange(int first, int last);
    /// @copydoc setSliceRange(int,int)
    void setSliceRange(const int range[2]);

    /// Set the path to the exported deep-learning model.
    void setModelPath(const QString& path);

    /// Run the model using the input image and get the output mask.
    void predict();

    /// Get the output mask predicted by the model (returns smart pointer).
    vtkSmartPointer<vtkImageData> getOutputMask() const;

protected:
    /// Input image on which the segmentation will take place.
    vtkImageData* m_inputVtkImage;
    /// Input dimension indices corresponding to the 3D axes.
    QVector<int> m_axes;
    /// Input reslice axes direction cosines.
    QVector<double> m_cosines;
    /// Dimensions expected by the model to predict (in column-major order:
    /// rows, columns, slices).
    QVector<int> m_modelDims;
    /// Number of channels expected by the model.
    int m_numChannels;
    /// Flag for normalisation.
    bool m_isNormalised;
    /// Number of output labels.
    int m_numLabels;
    /// Activation function for probabilities.
    ActivationFunction m_activationFunction;
    /// Path to the exported deep-learning model.
    QString m_modelPath;
    /// Slice range to be segmented (both ends included).
    QVector<int> m_sliceRange;
    /// Current slice being segmented.
    int m_currentSlice;
    /// Predicted mask using the deep-learning model.
    vtkSmartPointer<vtkImageData> m_outputMask;

    /// Prepare the reslicer to extract input images, and allocate mask.
    void setupInputOutput(vtkImageReslice* reslicer);

    /// Restore output mask according to input parameters (via reslicer).
    void resetOutput(vtkImageReslice* reslicer);

    /// Extract current slice from volume.
    void extractSlice(vtkImageReslice* reslicer, vtkImageData* extract) const;

    /// Load deep-learning model.
    virtual void loadModelParameters(){};

    /// Run the model using input data and store the result.
    virtual void run(float* data){};

    /// Interpret the result stored and fill the mask accordingly.
    virtual void fillMask(){};
};

} // namespace udg

#endif // UDG_DEEPLEARNINGSEGMENTATION_H
