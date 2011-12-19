#include "risrequestsportinusetest.h"
#include "inputoutputsettings.h"

namespace udg {

RISRequestsPortInUseTest::RISRequestsPortInUseTest(QObject *parent)
 : PortInUseTest(parent)
{
    m_port = Settings().getValue(InputOutputSettings::RISRequestsPort).toInt();
}

RISRequestsPortInUseTest::~RISRequestsPortInUseTest()
{
}

QString RISRequestsPortInUseTest::getDescription()
{
    return tr("RIS request port %1 is used by another application").arg(m_port);
}

} // End udg namespace
