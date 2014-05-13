/*************************************************************************************
  Copyright (C) 2014 Laboratori de Gràfics i Imatge, Universitat de Girona &
  Institut de Diagnòstic per la Imatge.
  Girona 2014. All rights reserved.
  http://starviewer.udg.edu

  This file is part of the Starviewer (Medical Imaging Software) open source project.
  It is subject to the license terms in the LICENSE file found in the top-level
  directory of this distribution and at http://starviewer.udg.edu/license. No part of
  the Starviewer (Medical Imaging Software) open source project, including this file,
  may be copied, modified, propagated, or distributed except according to the
  terms contained in the LICENSE file.
 *************************************************************************************/

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
