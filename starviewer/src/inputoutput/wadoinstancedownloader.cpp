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

#include "wadoinstancedownloader.h"

#include "dicomtagreader.h"
#include "directoryutilities.h"
#include "localdatabasemanager.h"
#include "logging.h"

#include <QDir>
#include <QFileInfo>

namespace udg {

namespace  {

// Creates the directory specified in the given file path including parent directories. Returns true if successful and false otherwise.
bool createFileDirectory(const QString &path)
{
    QFileInfo fileInfo(path);
    QString directoryToCreate = fileInfo.absolutePath();

    if (!QDir().mkpath(directoryToCreate))
    {
        ERROR_LOG(QString("Can't create directory %1.").arg(directoryToCreate));
        return false;
    }

    return true;
}

}

WadoInstanceDownloader::WadoInstanceDownloader()
    : m_instanceSize(0), m_totalBytesRead(0), m_dataSource(nullptr), m_finished(false)
{
}

void WadoInstanceDownloader::setStudyInstanceUid(const QString &studyInstanceUid)
{
    m_studyInstanceUid = studyInstanceUid;
    DirectoryUtilities::sanitizeFilename(m_studyInstanceUid);
}

const QString& WadoInstanceDownloader::getStudyInstanceUid() const
{
    return m_studyInstanceUid;
}

void WadoInstanceDownloader::setSeriesInstanceUid(const QString &seriesInstanceUid)
{
    m_seriesInstanceUid = seriesInstanceUid;
    DirectoryUtilities::sanitizeFilename(m_seriesInstanceUid);
}

const QString& WadoInstanceDownloader::getSeriesInstanceUid() const
{
    return m_seriesInstanceUid;
}

void WadoInstanceDownloader::setSopInstanceUid(const QString &sopInstanceUid)
{
    m_sopInstanceUid = sopInstanceUid;
    DirectoryUtilities::sanitizeFilename(m_sopInstanceUid);
}

const QString& WadoInstanceDownloader::getSopInstanceUid() const
{
    return m_sopInstanceUid;
}

void WadoInstanceDownloader::setInstanceSize(qint64 bytes)
{
    m_instanceSize = bytes;
}

void WadoInstanceDownloader::setDataSource(QIODevice *dataSource)
{
    m_dataSource = dataSource;
}

void WadoInstanceDownloader::read()
{
    if (m_file.fileName().isNull())
    {
        createFile();

        if (!m_file.isOpen())
        {
            throw std::runtime_error(qPrintable(QObject::tr("Can't save file to disk.")));
        }
    }

    qint64 bytesPendingToRead = m_instanceSize - m_totalBytesRead;
    qint64 bytesAvailable = m_dataSource->bytesAvailable();
    qint64 bytesToRead = std::min(bytesPendingToRead, bytesAvailable);

    if (bytesToRead > 0)
    {
        QByteArray data = m_dataSource->read(bytesToRead);
        m_totalBytesRead += data.size();
        m_file.write(data);
    }

    if (m_totalBytesRead == m_instanceSize)
    {
        m_finished = true;
        m_file.close();

        if (m_sopInstanceUid.isEmpty())
        {
            moveFileToFinalDestination();
        }
    }
}

bool WadoInstanceDownloader::hasFinished() const
{
    return m_finished;
}

QString WadoInstanceDownloader::getPath() const
{
    return m_file.fileName();
}

void WadoInstanceDownloader::createFile()
{
    QString path = LocalDatabaseManager::getCachePath();
    path += m_studyInstanceUid + "/";

    if (!m_seriesInstanceUid.isEmpty())
    {
        path += m_seriesInstanceUid + "/";

        if (!m_sopInstanceUid.isEmpty())
        {
            path += m_sopInstanceUid;
        }
    }

    if (m_sopInstanceUid.isEmpty())
    {
        QString randomFileName = QString("%1").arg(rand(), 8, 16, QChar('0'));
        path += randomFileName;
    }

    if (!createFileDirectory(path))
    {
        return;
    }

    m_file.setFileName(path);

    if (!m_file.open(QIODevice::WriteOnly))
    {
        ERROR_LOG(QString("Can't open file %1 for writing.").arg(path));
    }
}

void WadoInstanceDownloader::moveFileToFinalDestination()
{
    // In case of errors in this method we don't throw any exception. Just log the error, keep the random name and live with it.

    {   // Scope to ensure that dicomTagReader is destroyed, and thus the file closed, before renaming the file
        DICOMTagReader dicomTagReader(m_file.fileName());

        if (m_seriesInstanceUid.isEmpty())
        {
            setSeriesInstanceUid(dicomTagReader.getValueAttributeAsQString(DICOMSeriesInstanceUID));
        }

        setSopInstanceUid(dicomTagReader.getValueAttributeAsQString(DICOMSOPInstanceUID));
    }

    QString path = LocalDatabaseManager::getCachePath() + m_studyInstanceUid + "/" + m_seriesInstanceUid + "/" + m_sopInstanceUid;

    if (!createFileDirectory(path))
    {
        return;
    }

    if (QFile::exists(path))
    {
        QFile::remove(path);    // remove old file with same name (can happen if a study is redownloaded)
    }

    if (!m_file.rename(path))
    {
        ERROR_LOG(QString("Can't rename %1 to %2").arg(m_file.fileName()).arg(path));
    }
}

} // namespace udg
