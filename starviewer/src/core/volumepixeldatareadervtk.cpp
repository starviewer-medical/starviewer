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

#include "volumepixeldatareadervtk.h"

#include "logging.h"
#include "volumepixeldata.h"

#include <vtkEventQtSlotConnect.h>
#include <vtkImageReader2.h>
#include <vtkImageReader2Factory.h>
#include <vtkNew.h>
#include <vtkSmartPointer.h>
#include <vtkStringArray.h>

namespace udg {

VolumePixelDataReaderVtk::VolumePixelDataReaderVtk(QObject *parent)
    : VolumePixelDataReader(parent), m_reader(nullptr)
{
}

VolumePixelDataReaderVtk::~VolumePixelDataReaderVtk()
{
    if (m_reader)
    {
        m_reader->Delete();
    }
}

int VolumePixelDataReaderVtk::read(const QStringList &filenames)
{
    int errorCode = NoError;
    m_abortRequested = false;

    if (filenames.isEmpty())
    {
        WARN_LOG("Empty file list");
        return InvalidFileName;
    }

    m_reader = vtkImageReader2Factory::CreateImageReader2(qPrintable(filenames.first()));

    if (!m_reader)
    {
        WARN_LOG("No suitable vtkImageReader2 subclass found for " + filenames.first());
        return CannotReadFile;
    }

    vtkNew<vtkEventQtSlotConnect> vtkQtConnection;
    vtkQtConnection->Connect(m_reader, vtkCommand::ProgressEvent, this, SLOT(progressSlot()));

    // Convert QStringList to vtkStringArray
    if (filenames.size() > 1)
    {
        auto stringArray = vtkSmartPointer<vtkStringArray>::New();

        for (int i = 0; i < filenames.size(); i++)
        {
            stringArray->InsertNextValue(filenames.at(i).toStdString());
        }

        DEBUG_LOG("Reading multiple files with VTK");
        m_reader->SetFileNames(stringArray);
    }
    else
    {
        DEBUG_LOG("Reading a single file with VTK");
        m_reader->SetFileName(qPrintable(filenames.first()));
    }

    try
    {
        m_reader->Update();
    }
    catch (const std::bad_alloc &)
    {
        errorCode = OutOfMemory;
    }
    catch (const std::exception &exception)
    {
        WARN_LOG(QString("An exception was thrown while reading with %1: %2").arg(m_reader->GetClassName()).arg(exception.what()));
        errorCode = UnknownError;
    }
    catch (...)
    {
        WARN_LOG(QString("An exception was thrown while reading with %1").arg(m_reader->GetClassName()));
        errorCode = UnknownError;
    }

    if (m_abortRequested)
    {
        errorCode = ReadAborted;
    }

    emit progress(100);

    m_volumePixelData = new VolumePixelData();
    m_volumePixelData->setData(m_reader->GetOutput());

    m_reader->Delete();
    m_reader = nullptr;

    return errorCode;
}

void VolumePixelDataReaderVtk::requestAbort()
{
    if (m_reader)
    {
        m_abortRequested = true;
        m_reader->AbortExecuteOn();
    }
}

void VolumePixelDataReaderVtk::progressSlot()
{
    if (m_reader)
    {
        emit progress(static_cast<int>(m_reader->GetProgress() * 100));
    }
}

} // namespace udg
