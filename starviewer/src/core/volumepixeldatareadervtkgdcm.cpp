#include "volumepixeldatareadervtkgdcm.h"

#include "logging.h"

#ifdef VTK_GDCM_SUPPORT
#include <vtkGDCMImageReader.h>
#include <vtkStringArray.h>
#include <vtkEventQtSlotConnect.h>
#include <gdcmPixelFormat.h> // Només per qüestions d'informació de debug
#endif

namespace udg {

VolumePixelDataReaderVTKGDCM::VolumePixelDataReaderVTKGDCM(QObject *parent)
: VolumePixelDataReader(parent)
{
#ifdef VTK_GDCM_SUPPORT
    m_vtkGDCMReader = vtkGDCMImageReader::New();
    // Mantenim el sistema de coordenades com quan es llegeix amb itkGDCM
    m_vtkGDCMReader->FileLowerLeftOn();
    // Pel progress de vtk
    m_vtkQtConnections = vtkEventQtSlotConnect::New();
    m_vtkQtConnections->Connect(m_vtkGDCMReader, vtkCommand::ProgressEvent, this, SLOT(slotProgress()));
#endif
}

VolumePixelDataReaderVTKGDCM::~VolumePixelDataReaderVTKGDCM()
{
}

int VolumePixelDataReaderVTKGDCM::read(const QStringList &filenames)
{
    int errorCode = NoError;

#ifdef VTK_GDCM_SUPPORT
    
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
    this->setData(m_vtkGDCMReader->GetOutput());

    DEBUG_LOG(">>>>>>>>>>>>VTK GDCM READER - vtkImageData Output<<<<<<<<<<<<<<<<<<<");
    m_imageDataVTK->Print(std::cout);
    
    emit progress(100);
    
    
#else
    DEBUG_LOG("No podem llegir amb la interfície VTK-GDCM, no estan instal·lades!");
    // TODO Assignar l'errorCode?
#endif
    return errorCode;
}

void VolumePixelDataReaderVTKGDCM::slotProgress()
{
#ifdef VTK_GDCM_SUPPORT
    emit progress((int)(m_vtkGDCMReader->GetProgress()*100));
#endif
}

} // End namespace udg
