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

#ifndef UDGRISREQUESTSPORTINUSETEST_H
#define UDGRISREQUESTSPORTINUSETEST_H

#include "portinusetest.h"

namespace udg {

class RISRequestsPortInUseTest : public PortInUseTest {
Q_OBJECT
public:
    RISRequestsPortInUseTest(QObject *parent = 0);
    ~RISRequestsPortInUseTest();

    ///Retorna descripcio del test
    QString getDescription();
};

static DiagnosisTestFactoryRegister<RISRequestsPortInUseTest> registerRISRequestsPortInUseTest("RISRequestsPortInUseTest");

} // end namespace udg

#endif
