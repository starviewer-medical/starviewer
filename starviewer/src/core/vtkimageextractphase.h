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

#ifndef UDG_VTKIMAGEEXTRACTPHASE_H
#define UDG_VTKIMAGEEXTRACTPHASE_H

#include <vtkThreadedImageAlgorithm.h>

namespace udg {

/**
 * @brief The VtkImageExtractPhase class is a filter that returns the selected phase from a multi-phase image.
 *
 * Currently, all phases must have the same number of slices.
 */
class VtkImageExtractPhase : public vtkThreadedImageAlgorithm
{
public:
    static VtkImageExtractPhase* New();

    vtkTypeMacro(VtkImageExtractPhase, vtkThreadedImageAlgorithm)

    virtual void PrintSelf(std::ostream &os, vtkIndent indent) override;

    /// Returns the number of phases.
    int getNumberOfPhases() const;
    /// Sets the number of phases.
    void setNumberOfPhases(int numberOfPhases);

    /// Returns the current phase.
    int getPhase() const;
    /// Sets the current phase.
    void setPhase(int phase);

protected:
    VtkImageExtractPhase();
    virtual ~VtkImageExtractPhase();

    /// Copies all the input information to the output, except the whole extent which is divided by the number of phases in the z dimension.
    virtual int RequestInformation(vtkInformation *request, vtkInformationVector **inputVector, vtkInformationVector *outputVector) override;

    /// Sets the input update extent corresponding to the output update extent.
    virtual int RequestUpdateExtent(vtkInformation *request, vtkInformationVector **inputVector, vtkInformationVector *outputVector) override;

    /// Copies the selected phase from the input data to the output data.
    virtual void ThreadedRequestData(vtkInformation *request, vtkInformationVector **inputVector, vtkInformationVector *outputVector, vtkImageData ***inData,
                                     vtkImageData **outData, int outExtent[6], int threadId) override;

private:
    /// Returns true if this filter can extract a phase with the current values and the given input information, and false otherwise.
    bool canExtractPhase(vtkInformation *inInfo) const;

    /// Computes the input extent corresponding to the given output extent.
    void computeInputExtentFromOutputExtent(int inExtent[6], const int outExtent[6]) const;

private:
    /// Number of phases in the image.
    int m_numberOfPhases;
    /// The index of the phase to be extracted.
    int m_phase;

};

} // namespace udg

#endif // UDG_VTKIMAGEEXTRACTPHASE_H
