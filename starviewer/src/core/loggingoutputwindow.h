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

#ifndef UDG_LOGGINGOUTPUTWINDOW_H
#define UDG_LOGGINGOUTPUTWINDOW_H

#include <vtkOutputWindow.h>

namespace udg {

/**
 * Subclasse de vtkOutputWindow que redirigeix els missatges d'error de VTK cap al log d'Starviewer.
 *
 * Encara que al nom hi digui "Window", no mostra cap finestra (i vtkOutputWindow per si sola tampoc).
 */
class LoggingOutputWindow : public vtkOutputWindow {

public:

    static LoggingOutputWindow* New();

    virtual void DisplayText(const char*);
    virtual void DisplayErrorText(const char*);
    virtual void DisplayWarningText(const char*);
    virtual void DisplayGenericWarningText(const char*);
    virtual void DisplayDebugText(const char*);

protected:
    
    LoggingOutputWindow();
    virtual ~LoggingOutputWindow();

private:

    LoggingOutputWindow(const LoggingOutputWindow&);    // No s'implementa.
    void operator=(const LoggingOutputWindow&);         // No s'implementa.

};

}

#endif
