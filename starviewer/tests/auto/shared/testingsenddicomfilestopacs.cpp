#include "testingsenddicomfilestopacs.h"

#include "testingpacsconnection.h"

namespace testing {

TestingSendDICOMFilesToPACS::TestingSendDICOMFilesToPACS(const PacsDevice &pacsDevice) :
    SendDICOMFilesToPACS(pacsDevice)
{
}

PACSConnection* TestingSendDICOMFilesToPACS::createPACSConnection(const PacsDevice &pacsDevice) const
{
    Q_UNUSED(pacsDevice);
    return new TestingPACSConnection();
}

bool TestingSendDICOMFilesToPACS::storeSCU(T_ASC_Association *association, QString filePathToStore)
{
    Q_UNUSED(association)
    Q_UNUSED(filePathToStore)
    m_numberOfDICOMFilesSentSuccessfully++;
    return true;
}

}
