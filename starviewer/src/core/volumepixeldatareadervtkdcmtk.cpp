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

#include "volumepixeldatareadervtkdcmtk.h"

#include "logging.h"
#include "volumepixeldata.h"
#include "vtkdcmtkimagereader.h"

#include <QStringList>

#include <vtkEventQtSlotConnect.h>
#include <vtkStringArray.h>

namespace udg {

VolumePixelDataReaderVTKDCMTK::VolumePixelDataReaderVTKDCMTK(QObject *parent) :
    VolumePixelDataReader(parent)
{
    m_reader = VtkDcmtkImageReader::New();

    // VTK progress
    m_vtkQtConnections = vtkEventQtSlotConnect::New();
    m_vtkQtConnections->Connect(m_reader, vtkCommand::ProgressEvent, this, SLOT(progressSlot()));
}

VolumePixelDataReaderVTKDCMTK::~VolumePixelDataReaderVTKDCMTK()
{
    m_reader->Delete();
    m_vtkQtConnections->Delete();
}

int VolumePixelDataReaderVTKDCMTK::read(const QStringList &filenames)
{
    int errorCode = NoError;
    m_abortRequested = false;

    if (filenames.isEmpty())
    {
        WARN_LOG("Empty file list");
        return InvalidFileName;
    }

    // Convert QStringList to vtkStringArray
    if (filenames.size() > 1)
    {
        vtkStringArray *stringArray = vtkStringArray::New();
        
        for (int i = 0; i < filenames.size(); i++)
        {
            stringArray->InsertNextValue(filenames.at(i).toStdString());
        }

        DEBUG_LOG("Reading multiple files with VTK-DCMTK");
        m_reader->SetFileNames(stringArray);
        stringArray->Delete();
    }
    else
    {
        DEBUG_LOG("Reading a single file with VTK-DCMTK");
        m_reader->SetFileName(qPrintable(filenames.first()));
    }

    // Set frame numbers to the reader (needed for multiframe files)
    m_reader->setFrameNumbers(m_frameNumbers);

    try
    {
        m_reader->Update();
    }
    catch (const std::bad_alloc &)
    {
        errorCode = OutOfMemory;
    }
    catch (const VtkDcmtkImageReader::CantReadImageException &exception)
    {
        DEBUG_LOG(QString("Can't read image with VTK-DCMTK: %1").arg(exception.what()));
        WARN_LOG(QString("Can't read image with VTK-DCMTK: %1").arg(exception.what()));
        errorCode = CannotReadFile;
    }
    catch (const std::exception &exception)
    {
        DEBUG_LOG(QString("An exception was thrown while reading with VtkDcmtkImageReader: %1").arg(exception.what()));
        WARN_LOG(QString("An exception was thrown while reading with VtkDcmtkImageReader: %1").arg(exception.what()));
        errorCode = UnknownError;
    }
    catch (...)
    {
        DEBUG_LOG("An exception was thrown while reading with VtkDcmtkImageReader");
        WARN_LOG("An exception was thrown while reading with VtkDcmtkImageReader");
        errorCode = UnknownError;
    }

    if (m_abortRequested)
    {
        errorCode = ReadAborted;
    }

    emit progress(100);

    m_volumePixelData = new VolumePixelData();
    m_volumePixelData->setData(m_reader->GetOutput());

    return errorCode;
}

void VolumePixelDataReaderVTKDCMTK::requestAbort()
{
    m_abortRequested = true;
    m_reader->AbortExecuteOn();
}

void VolumePixelDataReaderVTKDCMTK::progressSlot()
{
    emit progress(static_cast<int>(m_reader->GetProgress() * 100));
}

} // end namespace udg
