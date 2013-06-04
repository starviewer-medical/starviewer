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
