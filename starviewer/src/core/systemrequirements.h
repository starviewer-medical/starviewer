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

#ifndef UDGSYSTEMREQUIREMENTS_H
#define UDGSYSTEMREQUIREMENTS_H

#include <QStringList>

namespace udg {

/**
    Guarda els requeriments mínims i recomenats de Starviewer. Conté purament els mètodes per obtenir-los.
*/
class SystemRequirements {
public:
    SystemRequirements();
    virtual ~SystemRequirements();

    virtual unsigned int getMinimumCPUNumberOfCores();
    virtual unsigned int getMinimumCPUFrequency();
    virtual QStringList getMinimumGPUOpenGLCompatibilities();
    virtual QString getMinimumGPUOpenGLVersion();
    virtual unsigned int getMinimumGPURAM();
    virtual unsigned int getMinimumHardDiskFreeSpace();
    virtual QString getMinimumOperatingSystemVersion();
    virtual unsigned int getMinimumOperatingSystemServicePackVersion(); // for Windows 7
    virtual unsigned int getMinimumRAMTotalAmount();
    virtual unsigned int getMinimumScreenWidth();
    virtual bool doesOpticalDriveNeedWriteCapabilities();

protected:
    unsigned int m_minimumNumberOfCores;
    unsigned int m_minimumCoreSpeed;
    unsigned int m_minimumGPURAM;
    QString m_minimumGPUOpenGLVersion;
    QString m_minimumOSVersion;
    unsigned int m_minimumServicePackVersion;
    unsigned int m_minimumRAM;
    unsigned int m_minimumScreenWidth;
    QStringList m_minimumOpenGLExtensions;
    unsigned int m_minimumDiskSpace;
    bool m_doesOpticalDriveNeedsToWrite;
};

}

#endif
