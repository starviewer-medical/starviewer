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

#ifndef UDGECHOTOPACSTEST_H
#define UDGECHOTOPACSTEST_H

#include "diagnosistest.h"
#include "diagnosistestfactoryregister.h"

namespace udg {

class PacsDevice;

class EchoToPACSTest : public DiagnosisTest {
Q_OBJECT
public:
    EchoToPACSTest(QObject *parent = 0);
    ~EchoToPACSTest() override;

    DiagnosisTestResult run() override;

    ///Retorna descripcio del test
    QString getDescription() const override;

protected:
    virtual QList<PacsDevice> getPacsDeviceList();
    virtual DiagnosisTestProblem echo(const PacsDevice &pacs);
};

static DiagnosisTestFactoryRegister<EchoToPACSTest> registerEchoToPACSTest("EchoToPACSTest");

} // end namespace udg

#endif
