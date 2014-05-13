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

#ifndef UDGAPPLICATIONVERSIONTEST_H
#define UDGAPPLICATIONVERSIONTEST_H

#include "diagnosistest.h"
#include "diagnosistestresult.h"
#include "diagnosistestfactoryregister.h"

#include <QObject>

namespace udg {

class ApplicationVersionTest : public DiagnosisTest {
Q_OBJECT
public:
    /// Constructor per defecte
    ApplicationVersionTest(QObject *parent = 0);
    /// Destructor
    ~ApplicationVersionTest();
    /// Mètode implementat de la interfície DiagnosisTest.
    /// DiagnosisTestResult valdrà OK quan l'aplicació estigui actualitzada, Warning si hi ha alguna actualització disponible,
    /// i Error si hi ha hagut algun error.
    DiagnosisTestResult run();
    /// Retorna la descripció del test
    QString getDescription();

signals:
    void startCheckForUpdates();

protected:
    virtual void onlineCheck();

protected:
    bool m_onlineCheckOk;
    bool m_newVersionAvailable;
    QString m_olineCheckerErrorDescription;
};

static DiagnosisTestFactoryRegister<ApplicationVersionTest> registerApplicationVersionTest("ApplicationVersionTest");

}

#endif
