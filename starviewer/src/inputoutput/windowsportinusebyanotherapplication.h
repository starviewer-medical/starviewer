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
