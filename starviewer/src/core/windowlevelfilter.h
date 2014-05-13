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

#ifndef WINDOWLEVELFILTER_H
#define WINDOWLEVELFILTER_H

#include "filter.h"

class vtkImageData;
class vtkImageMapToWindowLevelColors3;

namespace udg {

class TransferFunction;
class WindowLevel;

///    This filter applies a window level window to the input.
class WindowLevelFilter : public Filter {

public:
    WindowLevelFilter();
    virtual ~WindowLevelFilter();

    /// Sets the given vtkImageData as input of the filter
    void setInput(vtkImageData *input);
    /// Sets the given filter output as input of the filter
    void setInput(FilterOutput input);

    /// Sets the window-level window
    void setWindowLevel(const WindowLevel &windowLevel);
    /// Sets the window-level window
    void setWindowLevel(double window, double level);

    /// Sets the window value
    void setWindow(double window);
    /// Gets the window value
    double getWindow() const;

    /// Sets the level value
    void setLevel(double level);
    /// Gets the level value
    double getLevel() const;

    /// Sets the transfer funcion
    void setTransferFunction(const TransferFunction &transferFunction);
    /// Clears the transfer funcion
    void clearTransferFunction();

private:
    /// Returns the vtkAlgorithm used to implement the filter.
    virtual vtkAlgorithm* getVtkAlgorithm() const;

private:
    vtkImageMapToWindowLevelColors3* m_filter;

};

}

#endif
