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

#ifndef UDGPORTINUSETEST_H
#define UDGPORTINUSETEST_H

#include "diagnosistest.h"
#include "diagnosistestresult.h"
#include "diagnosistestfactoryregister.h"

namespace udg {

class PortInUse;

class PortInUseTest : public DiagnosisTest {
Q_OBJECT
public:
    ~PortInUseTest() override;

    DiagnosisTestResult run() override;

    ///Retorna descripcio del test
    virtual QString getDescription() const override;

protected:
    explicit PortInUseTest(QObject *parent = nullptr);
    virtual PortInUse* createPortInUse();
    virtual void checkIfPortIsInUse(PortInUse *portInUse);

protected:
    /// Port que es comprovarà si està en ús.
    int m_port;
};

} // end namespace udg

#endif
