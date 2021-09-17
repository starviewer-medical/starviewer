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

#ifndef UDG_LINUXSYSTEMINFORMATION_H
#define UDG_LINUXSYSTEMINFORMATION_H

#include "systeminformation.h"

namespace udg {

/**
 * @brief The LinuxSystemInformation class implements methods of obtaining system information specific to Linux systems.
 */
class LinuxSystemInformation : public SystemInformation
{
public:
    /// Returns total amount of RAM in MiB.
    unsigned int getRAMTotalAmount() override;

    /// Return CPU frequency in MHz for each CPU.
    QList<unsigned int> getCPUFrequencies() override;

    /// Returns GPU model name for each GPU.
    QStringList getGPUModel() override;
    /// Returns amount of RAM in MiB for each GPU.
    QList<unsigned int> getGPURAM() override;
    /// Returns driver version for each GPU.
    QStringList getGPUDriverVersion() override;

    /// Returns true if an optical drive in the system can write any media, and false otherwise.
    bool doesOpticalDriveHaveWriteCapabilities() override;

    /// Returns information about the desktop environment and compositor on Linux.
    QString getDesktopInformation() const override;
};

} // namespace udg

#endif // UDG_LINUXSYSTEMINFORMATION_H
