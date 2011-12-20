#ifndef UDGRUNDIAGNOSISTEST_H
#define UDGRUNDIAGNOSISTEST_H

#include <QObject>
#include <QList>
#include <QPair>

#include "diagnosistestresult.h"

namespace udg {

class DiagnosisTest;

/**
    Executa els DiagnosisTest passats per paràmetre i en retorna el resultat. Aquesta classe està pensada per ser executada de forma assíncrona per un altre Thread i notificar-nos
    quan s'han acabat d'executar tots els threads.
  */

class RunDiagnosisTest : public QObject {
Q_OBJECT

public:
    RunDiagnosisTest(QList<DiagnosisTest*> diagnosisTestToRun);

    /// Retorna els testos executats amb el seu resultat
    QList<QPair<DiagnosisTest *,DiagnosisTestResult> > getRunTests();

    /// Retorna els testos qeu s'executaran
    QList<DiagnosisTest *> getDiagnosisTestToRun() const;

public slots:
    /// Executa els testos
    void run();

signals:
    /// Signal que s'emet per indicar que es comença a executar el DiagnosisTest passat
    void runningDiagnosisTest(DiagnosisTest *);

    /// Signal que s'emet quan s'han executat tots els testos
    void finished();

private:
    QList<DiagnosisTest *> m_diagnosisTestsToRun;
    QList<QPair<DiagnosisTest *, DiagnosisTestResult> > m_executedDiagnosisTest;

};

#endif // UDGRUNDIAGNOSISTEST_H

}
