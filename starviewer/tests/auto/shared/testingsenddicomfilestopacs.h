#ifndef TESTINGSENDDICOMFILESTOPACS_H
#define TESTINGSENDDICOMFILESTOPACS_H

#include "senddicomfilestopacs.h"

using namespace udg;

namespace testing {

class TestingSendDICOMFilesToPACS : public SendDICOMFilesToPACS {

public:

    TestingSendDICOMFilesToPACS(const PacsDevice &pacsDevice);

private:

    virtual PACSConnection* createPACSConnection(const PacsDevice &pacsDevice) const;
    virtual bool storeSCU(T_ASC_Association *association, QString filePathToStore);

};

}

#endif // TESTINGSENDDICOMFILESTOPACS_H
