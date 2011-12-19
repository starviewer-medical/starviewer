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

QString IncomingDICOMConnectionsPortInUseTest::getDescription()
{
    return tr("Incoming DICOM Connections Port %1 is in use by another application").arg(m_port);
}

} // End udg namespace
