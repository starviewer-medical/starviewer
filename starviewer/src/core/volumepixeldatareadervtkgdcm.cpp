#include "volumepixeldatareadervtkgdcm.h"

#include "logging.h"

#include <vtkGDCMImageReader.h>
#include <vtkStringArray.h>
#include <vtkEventQtSlotConnect.h>
#include <vtkPointData.h>
#include <vtkLookupTable.h>
#include <vtkImageMapToColors16.h>
#include <vtkImageMapToColors.h>
#include <vtkImageYBRToRGB.h>
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
        vtkStringArray *stringArray = vtkStringArray::New();
        for (unsigned int i = 0; i < filenames.size(); i++)
        {
            stringArray->InsertNextValue(filenames.at(i).toStdString());
        }
        DEBUG_LOG("Llegim diversos arxius amb vtkGDCM");
        m_vtkGDCMReader->SetFileNames(stringArray);
        stringArray->Delete();
    }
    else
    {
        DEBUG_LOG("Llegim un sol arxiu amb vtkGDCM");
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
        errorCode = UnknownError;
    }

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
            DEBUG_LOG(QString("Scalar type not listed in switch: %1").arg(m_vtkGDCMReader->GetDataScalarType()));
            break;
    }

    // Processem les dades segons el tipus d'espai de color en el que estiguin definides
    // per finalment assignar les dades a l'objecte vtkImageData
    if (errorCode == NoError)
    {
        applyColorProcessing();
    }
    emit progress(100);

    return errorCode;
}

void VolumePixelDataReaderVTKGDCM::applyColorProcessing()
{
    //
    // Extret de gdcmviewer.cxx (gdcm\Utilities\VTK\Applications)
    //
    
    // In case of palette color, let's tell VTK to map color:
    // MONOCHROME1 is also implemented with a lookup table

    int imageFormat = m_vtkGDCMReader->GetImageFormat();
    if (imageFormat == VTK_LOOKUP_TABLE || imageFormat == VTK_INVERSE_LUMINANCE)
    {
        assert(m_vtkGDCMReader->GetOutput()->GetPointData()->GetScalars() && m_vtkGDCMReader->GetOutput()->GetPointData()->GetScalars()->GetLookupTable());
        // Convert to color:
        vtkLookupTable *lookupTable = m_vtkGDCMReader->GetOutput()->GetPointData()->GetScalars()->GetLookupTable();
        if (!lookupTable)
        {
            // This must be a Segmented Palette and on VTK 4.4 this is not supported
            DEBUG_LOG("Not implemented. You will not see the Color LUT");
        }

        if (lookupTable->IsA("vtkLookupTable16"))
        {
            DEBUG_LOG("Mapejem una LUT de 16 bits");
            vtkImageMapToColors16 *imageColorMapper16 = vtkImageMapToColors16::New();
            imageColorMapper16->SetInput(m_vtkGDCMReader->GetOutput());
            imageColorMapper16->SetLookupTable(m_vtkGDCMReader->GetOutput()->GetPointData()->GetScalars()->GetLookupTable());
            if (imageFormat == VTK_LOOKUP_TABLE)
            {
                DEBUG_LOG(">> Format RGB");
                imageColorMapper16->SetOutputFormatToRGB();
            }
            else if (imageFormat == VTK_INVERSE_LUMINANCE)
            {
                DEBUG_LOG(">> Format INVERSE LUMINANCE");
                imageColorMapper16->SetOutputFormatToLuminance();
            }
            imageColorMapper16->Update();
            m_vtkImageData = imageColorMapper16->GetOutput();
            imageColorMapper16->Register(m_vtkImageData);
            imageColorMapper16->Delete();
        }
        else
        {
            DEBUG_LOG("Mapejem una LUT de 8 bits");
            vtkImageMapToColors *imageColorMapper = vtkImageMapToColors::New();
            imageColorMapper->SetInput(m_vtkGDCMReader->GetOutput());
            imageColorMapper->SetLookupTable(m_vtkGDCMReader->GetOutput()->GetPointData()->GetScalars()->GetLookupTable());
            if (imageFormat == VTK_LOOKUP_TABLE)
            {
                DEBUG_LOG(">> Format RGB");
                imageColorMapper->SetOutputFormatToRGB();
            }
            else if (imageFormat == VTK_INVERSE_LUMINANCE)
            {
                DEBUG_LOG(">> Format INVERSE LUMINANCE");
                imageColorMapper->SetOutputFormatToLuminance();
            }
            imageColorMapper->Update();
            m_vtkImageData = imageColorMapper->GetOutput();
            imageColorMapper->Register(m_vtkImageData);
            imageColorMapper->Delete();
        }
    }
    else if (imageFormat == VTK_YBR)
    {
        DEBUG_LOG("Mapejem espai de color YBR a RGB");
        vtkImageYBRToRGB *ybrToRGBFilter = vtkImageYBRToRGB::New();
        ybrToRGBFilter->SetInput(m_vtkGDCMReader->GetOutput());
        ybrToRGBFilter->Update();
        m_vtkImageData = ybrToRGBFilter->GetOutput();
        ybrToRGBFilter->Register(m_vtkImageData);
        ybrToRGBFilter->Delete();
    }
    else if (imageFormat == VTK_RGB || imageFormat == VTK_RGBA)
    {
        DEBUG_LOG("Llegim directament de vtkGDCMImageReader. No cal aplicar cap mapeig adicional, les dades estan en format RGB/RGBA");
        // easy case !
        m_vtkImageData = m_vtkGDCMReader->GetOutput();
    }
    else
    {
        DEBUG_LOG(QString("Llegim directament de vtkGDCMImageReader. No cal aplicar cap mapeig adicional. Format d'imatge: %1").arg(imageFormat));
        m_vtkImageData = m_vtkGDCMReader->GetOutput();
    }
}
void VolumePixelDataReaderVTKGDCM::slotProgress()
{
    emit progress((int)(m_vtkGDCMReader->GetProgress()*100));
}

} // End namespace udg
