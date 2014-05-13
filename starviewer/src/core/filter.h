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

#ifndef FILTER_H
#define FILTER_H

#include "filteroutput.h"

class vtkAlgorithm;

namespace udg {

/**
    A Filter processes a set of inputs and produces an output of type FilterOutput.
    This is the base class for all filters, defining and implementing the minimum common interface.
 */
class Filter {

    // FilterOutput can access private members of Filter
    friend class FilterOutput;

public:
    virtual ~Filter();

    /// Updates the filter and its output.
    virtual void update();

    /// Returns the output of the filter.
    FilterOutput getOutput();

private:
    /// Returns the vtkAlgorithm used to implement the filter. If more than one vtkAlgorithm is used, it should return the one that produces the final output.
    /// This method is called by the default implementation of update() and by FilterOutput.
    virtual vtkAlgorithm* getVtkAlgorithm() const = 0;

};

}

#endif
