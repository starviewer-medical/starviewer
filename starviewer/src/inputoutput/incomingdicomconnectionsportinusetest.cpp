#include "incomingdicomconnectionsportinusetest.h"
#include "inputoutputsettings.h"

namespace udg {

IncomingDICOMConnectionsPortInUseTest::IncomingDICOMConnectionsPortInUseTest(QObject *parent)
 : PortInUseTest(parent)
{
    m_port = Settings().getValue(InputOutputSettings::IncomingDICOMConnectionsPort).toInt();
}

IncomingDICOMConnectionsPortInUseTest::~IncomingDICOMConnectionsPortInUseTest()
{
}


} // End udg namespace
