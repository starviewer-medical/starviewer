#include "portinusebyanotherapplication.h"
#ifdef WIN32
#include "windowsportinusebyanotherapplication.h"
#endif

#include <QObject>

namespace udg {

PortInUseByAnotherApplication::PortInUseByAnotherApplication()
{
}

PortInUseByAnotherApplication::~PortInUseByAnotherApplication()
{
}

PortInUseByAnotherApplication* PortInUseByAnotherApplication::newInstance()
{
#ifdef WIN32
    return new WindowsPortInUseByAnotherApplication();
#else
    return new PortInUseByAnotherApplication();
#endif
}

bool PortInUseByAnotherApplication::isPortInUseByAnotherApplication(int port, bool &error)
{
    Q_UNUSED(port);
    error = false;
    return false;
}

} // End udg namespace
