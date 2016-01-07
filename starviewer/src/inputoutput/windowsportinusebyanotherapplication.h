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

#ifndef UDGWINDOWSPORTINUSEBYANOTHERAPPLICATION_H
#define UDGWINDOWSPORTINUSEBYANOTHERAPPLICATION_H

#include "portinusebyanotherapplication.h"

// Per poder declarar PMIB_TCPTABLE_OWNER_MODULE
#include <windows.h>
#include <tcpmib.h>

namespace udg {

/**
    Classe que s'encarrega de comprovar si un port està en ús per una aplicació que no sigui starviewer, a windows.
  */
class WindowsPortInUseByAnotherApplication : public PortInUseByAnotherApplication {
public:
    WindowsPortInUseByAnotherApplication();
    ~WindowsPortInUseByAnotherApplication();

    bool isPortInUseByAnotherApplication(int port, bool &error);

protected:
    PMIB_TCPTABLE_OWNER_MODULE getTCPTables();
};

}  // end namespace udg

#endif
