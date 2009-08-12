/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *

 ***************************************************************************/

#include "isoimagefilecreator.h"

#include <QApplication>
#include <QProgressDialog>
#include <QProcess>
#include <QFile>
#include <QDir>
#include <QFileInfo>

#include "logging.h"

namespace udg {

IsoImageFileCreator::IsoImageFileCreator()
{
}

IsoImageFileCreator::IsoImageFileCreator(QString inputPath, QString outputIsoImageFilePath)
{
    m_inputPath = inputPath;
    m_outputIsoImageFilePath = outputIsoImageFilePath;
}

IsoImageFileCreator::~IsoImageFileCreator()
{
}

void IsoImageFileCreator::setIsoImageLabel( QString isoImageLabel )
{
    m_isoImageLabel = isoImageLabel;
}

void IsoImageFileCreator::setInputPath( QString inputPath )
{
    m_inputPath = inputPath;
}

void IsoImageFileCreator::setOutputIsoImageFilePath( QString outputIsoImageFilePath )
{
    m_outputIsoImageFilePath = outputIsoImageFilePath;
}

QString IsoImageFileCreator::getIsoImageLabel()
{
    return m_isoImageLabel;
}

QString IsoImageFileCreator::getInputPath()
{
    return m_inputPath;
}
    
QString IsoImageFileCreator::getOutputIsoImageFilePath()
{
    return m_outputIsoImageFilePath;
}

IsoImageFileCreator::ImageFileCreationError IsoImageFileCreator::getLastError()
{
    return m_lastError;
}

QString IsoImageFileCreator::getLastErrorDescription()
{
    return m_lastErrorDescription;
}

bool IsoImageFileCreator::createIsoImageFile()
{
    // Es comprova que el directori o fitxer a partir del qual es vol generar el fitxer d'imatge ISO existeix
    if( !QFile::exists(m_inputPath) )
    {
        m_lastErrorDescription = QObject::tr("The input path that we want to turn into an ISO image doesn't exist.");
        m_lastError = InputPathNotExist;
        return false;
    }

    QFileInfo outputIsoImageFilePathInfo(m_outputIsoImageFilePath);
    QFileInfo outputIsoImageDirPathInfo(outputIsoImageFilePathInfo.dir(),"");

    // Es comprova que el directori on es vol guardar el fitxer de imatge ISO existeixi
    if ( !outputIsoImageFilePathInfo.dir().exists() )
    {
        m_lastErrorDescription = QObject::tr("The directory where you want to save ISO image file doesn't exist.");
        m_lastError = OutputIsoImageFilePathNotExist;
        return false;
    }

    // Es comprova que es disposi de permisos d'escriptura en el directori on es vol guardar el fitxer d'imatge ISO
    // Es a dir, es comproven els permisos del directori.
    if ( !outputIsoImageDirPathInfo.isWritable() )
    {
        m_lastErrorDescription = QObject::tr("You don't have permissions to write in the output iso image directory path.");
        m_lastError = OutputIsoImageDirPathNotPermissions;
        return false;
    }

    // Es comprova que es diposi de permisos de escritura en el path on s'ha de crear el fitxer d'imatge ISO
    // Es a dir, es comproven els permisos del fitxer
    if ( !outputIsoImageFilePathInfo.isWritable() )
    {
        m_lastErrorDescription = QObject::tr("You don't have permissions to write in the output iso image file path.");
        m_lastError = OutputIsoImageFilePathNotExist;
        return false;
    }

    QProcess process;
    QStringList processParameters;

    processParameters << "-V"; 
    processParameters << "STARVIEWER DICOMDIR"; // Indiquem que el label de la imatge és STARVIEWER DICOMDIR
    processParameters << "-o"; 
    processParameters << m_outputIsoImageFilePath; // Nom i directori on guardarem la imatge
    processParameters << m_inputPath; // Path a convertir en iso

    QString mkisofsFilePath = QCoreApplication::applicationDirPath() + "/mkisofs";

    // Es comprova que existeixi el mkisofs al path on hauria d'estar per windows, mac i linux
    // Windows
    if ( !QFile::exists(mkisofsFilePath + ".exe") )
    {
        // Mac
        if ( !QFile::exists(mkisofsFilePath) )
        {
            // Linux
            mkisofsFilePath = "/usr/bin/mkisofs";
        
            // Si no existeix per cap sistema s'ha produit un error degut a que no s'ha pogut localitzar el mkisofs
            if ( !QFile::exists(mkisofsFilePath) )
            {
                m_lastErrorDescription = QObject::tr("mkisofs can be found in the system and it is necessary. You need to install it.");
                m_lastError = InternError;
            
                return false;
            }
        }
    }

    process.start(mkisofsFilePath, processParameters);
    
    process.waitForFinished( -1 ); // Esperem que s'hagi generat la imatge

    if( process.exitCode() != 0 )
    {
        m_lastErrorDescription = QObject::tr("An error occurred with the iso image file create process.");
        m_lastError = InternError;

        ERROR_LOG( "Error al crear ISO amb comanda: " + mkisofsFilePath + "; Amb paràmetres: " + processParameters.join(" ") + "; Exit code qprocess: " + process.exitCode() );
        return false;
    }
    return true;
}

}
