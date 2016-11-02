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

#ifndef IMAGEPIPELINE_H
#define IMAGEPIPELINE_H

#include "filter.h"

class vtkImageData;
class vtkRunThroughFilter;

namespace udg {

class PhaseFilter;
class TransferFunction;
class VoiLut;
class WindowLevelFilter;

/**
 * @brief The ImagePipeline class encapsulates the pipeline that applies the filters required to generate the image to be used in Q2DViewer.
 */
class ImagePipeline : public Filter {

public:
    ImagePipeline();
    virtual ~ImagePipeline();

    /// Sets the given vtkImageData as input of the filter
    void setInput(vtkImageData *input);
    /// Sets the given filter output as input of the filter
    void setInput(FilterOutput input);

    /// Sets the number of phases of the volume.
    void setNumberOfPhases(int numberOfPhases);
    /// Sets the phase that has to be displayed in the volume.
    void setPhase(int phase);
    /// Returns the output image data of the phase filter.
    vtkImageData* getPhaseOutput();

    /// Enables or disables window level and transfer function filter.
    void enableColorMapping(bool enable);
    /// Sets the VOI LUT.
    void setVoiLut(const VoiLut &voiLut);
    /// Sets the transfer function
    void setTransferFunction(const TransferFunction &transferFunction);
    /// Clears the transfer function.
    void clearTransferFunction();

private:
    /// Returns the vtkAlgorithm used to implement the filter.
    virtual vtkAlgorithm* getVtkAlgorithm() const;

    /// Rebuilds this pipeline choosing which filters to use according to their current status.
    void rebuild();

private:
    /// Filter to extract a single phase from a multi-phase volume.
    PhaseFilter *m_phaseFilter;
    /// Filter to apply a grayscale to volume
    WindowLevelFilter *m_windowLevelLUTFilter;
    /// Filter to obtain the final output of the pipeline
    vtkRunThroughFilter *m_outputFilter;

    /// Input data
    vtkImageData *m_input;

    /// Whether the window level filter is enabled.
    bool m_enableColorMapping;
    /// Used to keep track of whether there's a currently active transfer function when applying a VOI LUT.
    bool m_hasTransferFunction;

};

}
#endif // IMAGEPIPELINE_H
