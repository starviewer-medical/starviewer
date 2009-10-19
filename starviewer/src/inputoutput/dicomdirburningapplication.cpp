/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *

 ***************************************************************************/

#include "dicomdirburningapplication.h"

#include <QProcess>
#include <QFile>
#include <QDir>

#include "logging.h"
#include "inputoutputsettings.h"

namespace udg {

DICOMDIRBurningApplication::DICOMDIRBurningApplication()
{
}

DICOMDIRBurningApplication::DICOMDIRBurningApplication(const QString &isoPath, const CreateDicomdir::recordDeviceDicomDir &currentDevice)
{
    m_isoPath = isoPath;
    m_currentDevice = currentDevice;
}

DICOMDIRBurningApplication::~DICOMDIRBurningApplication()
{
}

void DICOMDIRBurningApplication::setIsoPath( const QString &isoPath )
{
    m_isoPath = isoPath;
}

QString DICOMDIRBurningApplication::getIsoPath() const
{
    return m_isoPath;
}

void DICOMDIRBurningApplication::setCurrentDevice(const CreateDicomdir::recordDeviceDicomDir &currentDevice)
{
    m_currentDevice = currentDevice;
}

CreateDicomdir::recordDeviceDicomDir DICOMDIRBurningApplication::getCurrentDevice() const
{
    return m_currentDevice;
}

DICOMDIRBurningApplication::DICOMDIRBurningApplicationError DICOMDIRBurningApplication::getLastError() const
{
    return m_lastError;
}

QString DICOMDIRBurningApplication::getLastErrorDescription() const
{
    return m_lastErrorDescription;
}

bool DICOMDIRBurningApplication::burnIsoImageFile()
{
    // Es comprova que el fitxer iso que es vol gravar en CD o DVD existeixi
    if( !QFile::exists(m_isoPath) )
    {
        m_lastErrorDescription = QObject::tr("The iso path that we want to burn doesn't exist.");
        m_lastError = IsoPathNotFound;
        return false;
    }

    Settings settings;
    QString burningApplicationPath = (settings.getValue(InputOutputSettings::DICOMDIRBurningApplicationPathKey)).toString();

    // Es comprova que el path de l'aplicació de gravar sigui correcte, tot i que en principi s'ha validat en la configuració del DICOMDIR
    if( !QFile::exists(burningApplicationPath) )
    {
        m_lastErrorDescription = QObject::tr("The burn application path doesn't exist.");
        m_lastError = BurnApplicationPathNotFound;
        return false;
    }

    QProcess process;
    QStringList processParameters;
   
    // Si està activada la opció d'entrar diferents paràmetres segons si es vol gravar un CD o un DVD caldrà afegir-los al processParameters
    if( (settings.getValue(InputOutputSettings::DICOMDIRBurningApplicationHasDifferentCDDVDParametersKey)).toBool() )
    {
        switch ( m_currentDevice )
        {
            case CreateDicomdir::CdRom :
                processParameters << (settings.getValue(InputOutputSettings::DICOMDIRBurningApplicationCDParametersKey)).toString().arg(QDir::toNativeSeparators(m_isoPath)).split(" ");
                break;
            case CreateDicomdir::DvdRom :
                processParameters << (settings.getValue(InputOutputSettings::DICOMDIRBurningApplicationDVDParametersKey)).toString().arg(QDir::toNativeSeparators(m_isoPath)).split(" ");
                break;
            default :
                break;
        }
    }
    else
    {
        processParameters << (settings.getValue(InputOutputSettings::DICOMDIRBurningApplicationParametersKey)).toString().arg(QDir::toNativeSeparators(m_isoPath)).split(" ");
    }

    process.start(burningApplicationPath, processParameters);
    process.waitForFinished( -1 );

    if( process.exitCode() != 0 )
    {
        m_lastErrorDescription = QObject::tr("An error occurred with the iso image file burn process.");
        m_lastError = InternalError;

        ERROR_LOG( "Error al gravar la imatge ISO amb comanda: " + burningApplicationPath + "; Amb paràmetres: " + processParameters.join(" ") + "; Exit code qprocess: " + process.exitCode() );
        return false;
    }
    return true;
}

}
