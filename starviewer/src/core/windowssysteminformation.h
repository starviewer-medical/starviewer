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

#ifndef UDGWINDOWSSYSTEMINFORMATION_H
#define UDGWINDOWSSYSTEMINFORMATION_H

#include "systeminformation.h"
// Qt
#include <QStringList>
// Windows
#include <Wbemidl.h>

namespace udg {

class WindowsSystemInformation : public SystemInformation {
public:
    WindowsSystemInformation();
    ~WindowsSystemInformation() override;
    
    OperatingSystem getOperatingSystem() override;
    bool isOperatingSystem64BitArchitecture();
    QString getOperatingSystemVersion() override;
    QString getOperatingSystemServicePackVersion() override;
    QString getOperatingSystemName();
    QString getOperatingSystemAsString() override;
    QString getOperatingSystemAsShortString() override;

    /// Retorna la quantitat total de memòria RAM en MegaBytes
    unsigned int getRAMTotalAmount() override;
    QList<unsigned int> getRAMModulesCapacity() override;
    QList<unsigned int> getRAMModulesFrequency() override;

    unsigned int getCPUNumberOfCores() override;
    
    /// Retorna una llista amb la freqüència de cada processador 
    QList<unsigned int> getCPUFrequencies() override;
    unsigned int getCPUL2CacheSize() override;

    QStringList getGPUBrand() override;
    QStringList getGPUModel() override;
    QList<unsigned int> getGPURAM() override;
    QStringList getGPUDriverVersion() override;

    // Screen, Display, Monitor, Desktop, ...
    QStringList getScreenVendors() override;

    QStringList getHardDiskDevices() override;
    unsigned int getHardDiskCapacity(const QString &device) override; // Del disc dur que conté la carpeta de la cache de Starviewer
    bool doesOpticalDriveHaveWriteCapabilities() override;

    unsigned int getNetworkAdapterSpeed() override;

    bool isDesktopCompositionAvailable() override;
    bool isDesktopCompositionEnabled() override;

protected:
    /// Mètode alternatiu per si no podem obtenir el nombre de nuclis via WMI
    virtual unsigned int getCPUNumberOfCoresFromEnvironmentVar();

    /// Gets the major version of the service pack
    QString getOperatingSystemServicePackMajorVersion();

    /// Gets the minor version of the service pack
    QString getOperatingSystemServicePackMinorVersion();

protected:
    virtual IWbemClassObject* getNextObject(IEnumWbemClassObject *enumerator);
    virtual IEnumWbemClassObject* executeQuery(QString query);
    virtual bool getProperty(IWbemClassObject *object, QString propertyName, VARIANT *propertyVariant);
    IWbemServices* initializeAPI();
    virtual void uninitializeAPI(IWbemServices *services);

protected:
    IWbemServices* m_api;

private:
    /// Returns a pointer to Dwmapi.dll if available, null otherwise
    HMODULE getDesktopWindowManagerDLL();

private:
    /// Constant string for Dwmapi.dll
    static const LPWSTR DesktopWindowManagerDLLName;

    /// Pointer to DwmIsCompositionEnabled function address (Dwmapi.dll)
    typedef HRESULT (WINAPI* DwmIsCompositionEnabledType)(BOOL*);
};

}

#endif
