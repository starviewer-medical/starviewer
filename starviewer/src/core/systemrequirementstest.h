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

#ifndef UDGSYSTEMREQUIREMENTSTEST_H
#define UDGSYSTEMREQUIREMENTSTEST_H

#include "diagnosistest.h"
#include "diagnosistestresult.h"
#include "diagnosistestfactoryregister.h"
#include "systeminformation.h"

class QString;
class QSize;

namespace udg {

class SystemRequirements;

/**
    Test de diagnosis de l'aplicació que comprova si el sistema compleix els requeriments mínims per que Starviewer funcioni correctament.
*/
class SystemRequirementsTest : public DiagnosisTest {
Q_OBJECT
public:
    SystemRequirementsTest(QObject *parent = 0);
    ~SystemRequirementsTest();

    DiagnosisTestResult run();

    /// Retorna la descripció del test
    QString getDescription();

protected:
    enum VersionComparison { Older, Same, Newer };

    /// Retorna 0 sí son iguals, 1 sí la version1 és major que la version2 i -1 si és menor.
    VersionComparison compareVersions(QString version1, QString version2);

    virtual unsigned int getCPUNumberOfCores(SystemInformation *system);
    virtual QList<unsigned int> getCPUFrequencies(SystemInformation *system);
    virtual QStringList getGPUOpenGLCompatibilities(SystemInformation *system);
    virtual QString getGPUOpenGLVersion(SystemInformation *system);
    virtual QList<unsigned int> getGPURAM(SystemInformation *system);
    virtual QStringList getGPUModel(SystemInformation *system);
    virtual unsigned int getHardDiskFreeSpace(SystemInformation *system, const QString &device);
    virtual SystemInformation::OperatingSystem getOperatingSystem(SystemInformation *system);
    virtual QString getOperatingSystemVersion(SystemInformation *system);
    virtual QString getOperatingSystemServicePackVersion(SystemInformation *system);
    virtual bool isOperatingSystem64BitArchitecture(SystemInformation *system);
    virtual unsigned int getRAMTotalAmount(SystemInformation *system);
    virtual QList<QSize> getScreenResolutions(SystemInformation *system);
    virtual bool doesOpticalDriveHaveWriteCapabilities(SystemInformation *system);

    virtual SystemRequirements* getSystemRequirements();
};

static DiagnosisTestFactoryRegister<SystemRequirementsTest> registerSystemRequirementsTest("SystemRequirementsTest");

}

#endif
