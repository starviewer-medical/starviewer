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
#include "diagnosistestresult.h"
#include "diagnosistestfactoryregister.h"
#include "pacsdevice.h"
#include "echotopacs.h"

namespace udg {

class EchoToPACSTest : public DiagnosisTest {
Q_OBJECT
public:
    EchoToPACSTest(QObject *parent = 0);
    ~EchoToPACSTest();

    DiagnosisTestResult run();

    ///Retorna descripcio del test
    QString getDescription();

protected:
    virtual QList<PacsDevice> getPacsDeviceList();
    virtual EchoToPACS::EchoRequestStatus echo(const PacsDevice &pacs);
};

static DiagnosisTestFactoryRegister<EchoToPACSTest> registerEchoToPACSTest("EchoToPACSTest");

} // end namespace udg

#endif
