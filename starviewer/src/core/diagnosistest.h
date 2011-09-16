#ifndef UDGDIAGNOSISTEST_H
#define UDGDIAGNOSISTEST_H

#include "diagnosistestresult.h"

namespace udg {

class DiagnosisTest {
public:
    /// Destructor per defecte. És virtual per que les subclasses puguin reimplementar-lo
    virtual ~DiagnosisTest(){}
    /// Mètode principal de la interfície. No està implementat, i es força a les subclasses a fer-ho.
    virtual DiagnosisTestResult run() = 0;
protected:
    /// Constructor per defecte protected perque no es pugui instanciar objectes DiagnosisTest
    DiagnosisTest(){}
};

}

#endif
