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

#ifndef UDGCACHETEST_H
#define UDGCACHETEST_H

#include "diagnosistest.h"
#include "diagnosistestresult.h"
#include "diagnosistestfactoryregister.h"

namespace udg {

class CacheTest : public DiagnosisTest {
Q_OBJECT
public:
    CacheTest(QObject *parent = 0);
    ~CacheTest();

    DiagnosisTestResult run();

    ///Retorna descripcio del test
    QString getDescription();

protected:
    virtual unsigned int getFreeSpace(const QString &cachePath);
    virtual unsigned int getMinimumFreeSpace();
    virtual bool doesCacheDirectoryHaveReadWritePermissions(const QString &cachePath);
    virtual bool isCacheOnDefaultPath();
};

static DiagnosisTestFactoryRegister<CacheTest> registerCacheTest("CacheTest");

} // end namespace udg

#endif
