#include "volumepixeldatareadervtkgdcm.h"

#include "logging.h"

#include <vtkGDCMImageReader.h>
#include <vtkStringArray.h>
#include <vtkEventQtSlotConnect.h>
#include <gdcmPixelFormat.h> // Només per qüestions d'informació de debug
// Qt
#include <QStringList>

namespace udg {

VolumePixelDataReaderVTKGDCM::VolumePixelDataReaderVTKGDCM(QObject *parent)
: VolumePixelDataReader(parent)
{
    m_vtkGDCMReader = vtkGDCMImageReader::New();
    // Mantenim el sistema de coordenades com quan es llegeix amb itkGDCM
    m_vtkGDCMReader->FileLowerLeftOn();
    // Pel progress de vtk
    m_vtkQtConnections = vtkEventQtSlotConnect::New();
    m_vtkQtConnections->Connect(m_vtkGDCMReader, vtkCommand::ProgressEvent, this, SLOT(slotProgress()));
}

VolumePixelDataReaderVTKGDCM::~VolumePixelDataReaderVTKGDCM()
{
}

int VolumePixelDataReaderVTKGDCM::read(const QStringList &filenames)
{
    int errorCode = NoError;

    if (filenames.isEmpty())
    {
        WARN_LOG("La llista de noms de fitxer per carregar és buida");
        errorCode = InvalidFileName;
        return errorCode;
    }

    // vtk - GDCM
    // Convertim la QStringList a vtkStringArray que és l'input 
    // que accepta vtkGDCMImageReader
    if (filenames.size() > 1)
    {
        vtkStringArray *sarray = vtkStringArray::New();
        for (unsigned int i = 0; i<filenames.size(); i++)
        {
            sarray->InsertNextValue(filenames.at(i).toStdString());
        }
        DEBUG_LOG("1)Reading several files");
        m_vtkGDCMReader->SetFileNames(sarray);
        sarray->Delete();
    }
    else
    {
        DEBUG_LOG("1)Reading only one file");
        m_vtkGDCMReader->SetFileName(qPrintable(filenames.first()));    
    }
    try
    {
        m_vtkGDCMReader->Update();
    }
    catch (...)
    {
        DEBUG_LOG("An exception was throwed while reading with vtkGDCMImageReader");
        WARN_LOG("An exception was throwed while reading with vtkGDCMImageReader");
    }
    DEBUG_LOG("2)Reading successful");
    
    DEBUG_LOG("Scalar type selected by the reader");
    switch (m_vtkGDCMReader->GetDataScalarType())
    {
        case gdcm::PixelFormat::INT16:
            DEBUG_LOG("INT 16");
            break;

        case gdcm::PixelFormat::UINT16:
            DEBUG_LOG("unsigned INT 16");
            break;

        case gdcm::PixelFormat::UINT8:
            DEBUG_LOG("unsigned INT 8");
            break;

        case gdcm::PixelFormat::INT8:
            DEBUG_LOG("INT 8");
            break;

        case gdcm::PixelFormat::UINT12:
            DEBUG_LOG("unsigned INT 12");
            break;

        case gdcm::PixelFormat::INT12:
            DEBUG_LOG("INT 12");
            break;

        case gdcm::PixelFormat::UINT32: // For some DICOM files (RT or SC)
            DEBUG_LOG("unsigned INT 32");
            break;

        case gdcm::PixelFormat::INT32: //    "   "
            DEBUG_LOG("INT 32");
            break;

        case gdcm::PixelFormat::FLOAT16: // sure why not...
            DEBUG_LOG("FLOAT 16");
            break;

        case gdcm::PixelFormat::FLOAT32: // good ol' 'float'
            DEBUG_LOG("FLOAT 32");
            break;

        case gdcm::PixelFormat::FLOAT64: // aka 'double'
            DEBUG_LOG("FLOAT 64");
            break;

        case gdcm::PixelFormat::UNKNOWN:
            DEBUG_LOG("UNKNOWN");
            break;

        default:
            DEBUG_LOG(QString("Scalar type: %1").arg(m_vtkGDCMReader->GetDataScalarType()));
            break;
    }

    // Assignem les dades
    m_vtkImageData = m_vtkGDCMReader->GetOutput();

    DEBUG_LOG(">>>>>>>>>>>>VTK GDCM READER - vtkImageData Output<<<<<<<<<<<<<<<<<<<");
    m_vtkImageData->Print(std::cout);
    
    emit progress(100);

    return errorCode;
}

void VolumePixelDataReaderVTKGDCM::slotProgress()
{
    emit progress((int)(m_vtkGDCMReader->GetProgress()*100));
}

} // End namespace udg
