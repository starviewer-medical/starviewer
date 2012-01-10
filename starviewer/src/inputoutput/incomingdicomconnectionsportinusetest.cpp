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
    return tr("Incoming DICOM connections port is available");
}

} // End udg namespace
