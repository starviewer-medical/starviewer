#ifndef UDGSYSTEMREQUERIMENTS_H
#define UDGSYSTEMREQUERIMENTS_H

#include <QString>
#include <QList>

namespace udg {

/**
    Guarda els requeriments mínims i recomenats de Starviewer. Conté purament els mètodes per obtenir-los.
*/
class SystemRequeriments {
public:
    SystemRequeriments();
    ~SystemRequeriments();

    virtual unsigned int getMinimumCPUNumberOfCores();
    virtual unsigned int getMinimumCPUFrequency();
    virtual QList<QString> getMinimumGPUOpenGLCompatibilities();
    virtual QString getMinimumGPUOpenGLVersion();
    virtual unsigned int getMinimumGPURAM();
    virtual unsigned int getMinimumHardDiskFreeSpace();
    virtual QString getMinimumOperatingSystemVersion();
    // TODO S'assumeix que el sistema operatiu al que ens referim pel service pack és Windows XP
    // Caldria fer un mètode més genèric per contemplar altres sistemes operatius Windows, com 7
    virtual unsigned int getMinimum32bitOperatingSystemServicePackVersion();
    virtual unsigned int getMinimum64bitOperatingSystemServicePackVersion();
    virtual bool doesOperatingSystemNeedToBe64BitArchitecutre();
    virtual unsigned int getMinimumRAMTotalAmount();
    virtual unsigned int getMinimumScreenWidth();
    virtual bool doesOpticalDriveNeedWriteCapabilities();

protected:
    unsigned int m_minimumNumberOfCores;
    unsigned int m_minimumCoreSpeed;
    unsigned int m_minimumGPURAM;
    QString m_minimumGPUOpenGLVersion;
    QString m_minimumOSVersion;
    unsigned int m_minimum32bitServicePackVersion;
    unsigned int m_minimum64bitServicePackVersion;
    bool m_doesOperatingSystemNeedToBe64BitArchitecutre;
    unsigned int m_minimumRAM;
    unsigned int m_minimumScreenWidth;
    QList<QString> m_minimumOpenGLExtensions;
    unsigned int m_minimumDiskSpace;
    bool m_doesOpticalDriveNeedsToWrite;
};

}

#endif
