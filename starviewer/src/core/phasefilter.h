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

#ifndef UDG_PHASEFILTER_H
#define UDG_PHASEFILTER_H

#include "filter.h"

namespace udg {

class VtkImageExtractPhase;

/**
 * @brief The PhaseFilter class is a Filter that extracts one phase from a multi-phase volume.
 */
class PhaseFilter : public Filter
{
public:
    PhaseFilter();
    virtual ~PhaseFilter();

    /// Sets the given vtkImageData as input of the filter.
    void setInput(vtkImageData *input);
    /// Sets the given filter output as input of the filter.
    void setInput(FilterOutput input);

    /// Returns the number of phases.
    int getNumberOfPhases() const;
    /// Sets the number of phases.
    void setNumberOfPhases(int numberOfPhases);

    /// Returns the current phase.
    int getPhase() const;
    /// Sets the current phase.
    void setPhase(int phase);

private:
    /// Returns the vtkAlgorithm used to implement the filter.
    virtual vtkAlgorithm* getVtkAlgorithm() const override;

private:
    /// The VTK filter used to implement this filter.
    VtkImageExtractPhase *m_filter;

};

} // namespace udg

#endif // UDG_PHASEFILTER_H
