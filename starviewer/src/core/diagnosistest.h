#ifndef UDGDIAGNOSISTEST_H
#define UDGDIAGNOSISTEST_H

#include "diagnosistestresult.h"
#include <QObject>

namespace udg {

class DiagnosisTest : public QObject {
public:
    /// Destructor per defecte. És virtual per que les subclasses puguin reimplementar-lo
    virtual ~DiagnosisTest(){}
    /// Mètode principal de la interfície. No està implementat, i es força a les subclasses a fer-ho.
    virtual DiagnosisTestResult run() = 0;
    /// Retorna la descripció del test
    virtual QString getDescription() = 0;
protected:
    /// Constructor per defecte protected perque no es pugui instanciar objectes DiagnosisTest
    DiagnosisTest(QObject *parent = 0) : QObject(parent){}
};

}

#endif
