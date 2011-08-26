#ifndef UDGDIAGNOSISTEST_H
#define UDGDIAGNOSISTEST_H

#include "diagnosistestresult.h"

namespace udg {

class DiagnosisTest {
public:
    /// Mètode principal de la interfície. No està implementat, i es força a les subclasses a fer-ho.
    virtual DiagnosisTestResult run() = 0;
protected:
    /// Destructor per defecte. És virtual per que les subclasses puguin reimplementar-lo
    virtual ~DiagnosisTest(){};
};

}

#endif
