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

IsoImageFileCreator::IsoImageFileCreator(const QString &inputPath, const QString &outputIsoImageFilePath)
{
    m_inputPath = inputPath;
    m_outputIsoImageFilePath = outputIsoImageFilePath;
}

IsoImageFileCreator::~IsoImageFileCreator()
{
}

QString IsoImageFileCreator::getIsoImageLabel() const
{
    return m_isoImageLabel;
}

void IsoImageFileCreator::setIsoImageLabel(const QString &isoImageLabel)
{
    m_isoImageLabel = isoImageLabel;
}

QString IsoImageFileCreator::getInputPath() const
{
    return m_inputPath;
}

void IsoImageFileCreator::setInputPath(const QString &inputPath)
{
    m_inputPath = inputPath;
}

QString IsoImageFileCreator::getOutputIsoImageFilePath() const
{
    return m_outputIsoImageFilePath;
}

void IsoImageFileCreator::setOutputIsoImageFilePath(const QString &outputIsoImageFilePath)
{
    m_outputIsoImageFilePath = outputIsoImageFilePath;
}

IsoImageFileCreator::ImageFileCreationError IsoImageFileCreator::getLastError() const
{
    return m_lastError;
}

QString IsoImageFileCreator::getLastErrorDescription() const
{
    return m_lastErrorDescription;
}

void IsoImageFileCreator::startCreateIsoImageFile()
{
    // Es comprova que el directori o fitxer a partir del qual es vol generar el fitxer d'imatge ISO existeix
    if (!QFile::exists(m_inputPath))
    {
        m_lastErrorDescription = QObject::tr("The input path \"%1\" to create the ISO image does not exist.").arg(m_inputPath);
        m_lastError = InputPathNotFound;
        emit finishedCreateIsoImageFile(false);
    }
    else
    {
        QFileInfo outputIsoImageFilePathInfo(m_outputIsoImageFilePath);
        QFileInfo outputIsoImageDirPathInfo(outputIsoImageFilePathInfo.dir(), "");

        // Es comprova que el directori on es vol guardar el fitxer de imatge ISO existeixi
        if (!outputIsoImageFilePathInfo.dir().exists())
        {
            m_lastErrorDescription = QObject::tr("The directory \"%1\" to save the ISO image file does not exist.")
                                                .arg(QDir::toNativeSeparators(outputIsoImageFilePathInfo.absolutePath()));
            m_lastError = OutputPathNotFound;
            emit finishedCreateIsoImageFile(false);
        }
        else
        {
            // Es comprova que es disposi de permisos d'escriptura en el directori on es vol guardar el fitxer d'imatge ISO
            // Es a dir, es comproven els permisos del directori.
            if (!outputIsoImageDirPathInfo.isWritable())
            {
                m_lastErrorDescription = QObject::tr("You don't have write permission in the output ISO image directory \"%1\".")
                                                    .arg(QDir::toNativeSeparators(outputIsoImageFilePathInfo.absolutePath()));
                m_lastError = OutputDirPathAccessDenied;
                emit finishedCreateIsoImageFile(false);
            }
            else
            {
                // Es comprova que es diposi de permisos de escritura en el path on s'ha de crear el fitxer d'imatge ISO
                // Es a dir, es comproven els permisos del fitxer
                if (QFile::exists(outputIsoImageFilePathInfo.absoluteFilePath()) && !outputIsoImageFilePathInfo.isWritable())
                {
                    m_lastErrorDescription = QObject::tr("You don't have permissions to write in the output ISO image directory \"%1\".")
                                                        .arg(QDir::toNativeSeparators(outputIsoImageFilePathInfo.absolutePath()));
                    m_lastError = OutputFilePathAccessDenied;
                    emit finishedCreateIsoImageFile(false);
                }
                else
                {
                    QStringList processParameters;

                    // Afegim als paràmetres l'etiqueta de la imatge ISO en cas de que tingui valor vàlid
                    if (!m_isoImageLabel.isEmpty())
                    {
                        processParameters << "-V";
                        processParameters << m_isoImageLabel;
                    }

                    // Per permetre conservar noms llargs de fitxers de més de 8 caracters a part d'ISO 9660 Level 1,
                    // s'afegeix informació en Joliet (Windows) i RockRidge (Unix)
                    // Això donava problemes al afegir visors propis i aquests tinguessin dll's amb noms més llargs, per exemple.
                    processParameters << "-joliet";
                    processParameters << "-r";

                    processParameters << "-o";
                    // Nom i directori on guardarem la imatge
                    processParameters << m_outputIsoImageFilePath;
                    // Path a convertir en iso
                    processParameters << m_inputPath;

                    QString mkisofsFilePath = QCoreApplication::applicationDirPath() + "/mkisofs";

                    // Es comprova que existeixi el mkisofs al path on hauria d'estar per windows, mac i linux
                    // Windows
                    if (!QFile::exists(mkisofsFilePath + ".exe"))
                    {
                        // Mac
                        if (!QFile::exists(mkisofsFilePath))
                        {
                            // Linux
                            mkisofsFilePath = "/usr/bin/mkisofs";

                            // Si no existeix per cap sistema s'ha produit un error degut a que no s'ha pogut localitzar el mkisofs
                            if (!QFile::exists(mkisofsFilePath))
                            {
                                m_lastErrorDescription = QObject::tr("The mkisofs program cannot be found on the system.");
                                m_lastError = InternalError;

                                emit finishedCreateIsoImageFile(false);
                            }
                        }
                    }
                    else
                    {
                        // Es crea el process i es connecta el seu signal finished amb l'slot finishCreationProcess
                        m_process = new QProcess();
                        connect(m_process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(finishCreationProcess(int)));
                        m_process->start(mkisofsFilePath, processParameters);
                    }
                }
            }
        }
    }
}

void IsoImageFileCreator::finishCreationProcess(int exitCode)
{
    // Després de que s'emeti el signal finished del QProcess, s'executa aquest Slot on es comprova si el procés a finialitzat o no
    // correctament i s'emet el signal finishedCreateIsoImageFile(bool)
    if (exitCode != 0)
    {
        m_lastErrorDescription = QObject::tr("An error occurred during the ISO image file creation process.");
        m_lastError = InternalError;

        ERROR_LOG(QString("Error al crear ISO; Exit code qprocess: %1").arg(exitCode));
        emit finishedCreateIsoImageFile(false);
    }
    else
    {
        emit finishedCreateIsoImageFile(true);
    }
}

}
