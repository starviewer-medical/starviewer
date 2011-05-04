#include "volumepixeldatareadervtkgdcm.h"

#include "logging.h"
#include "volumepixeldata.h"

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
    // Deshabilitem la lectura d'overlays. L'haurem de reactivar quan volguem suportar-los (#1358).
    m_vtkGDCMReader->LoadOverlaysOff();
    // Deshabilitem la lectura de les icon image. Reactivar-la si es necessita.
    m_vtkGDCMReader->LoadIconImageOff();
    // Pel progress de vtk
    m_vtkQtConnections = vtkEventQtSlotConnect::New();
    m_vtkQtConnections->Connect(m_vtkGDCMReader, vtkCommand::ProgressEvent, this, SLOT(slotProgress()));
    m_abortRequested = false;
}

VolumePixelDataReaderVTKGDCM::~VolumePixelDataReaderVTKGDCM()
{
    m_vtkGDCMReader->Delete();
    m_vtkQtConnections->Delete();
}

void VolumePixelDataReaderVTKGDCM::requestAbort()
{
    m_abortRequested = true;
    m_vtkGDCMReader->AbortExecuteOn();
}

int VolumePixelDataReaderVTKGDCM::read(const QStringList &filenames)
{
    int errorCode = NoError;
    m_abortRequested = false;

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
        for (int i = 0; i < filenames.size(); i++)
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
    catch (std::bad_alloc)
    {
        errorCode = OutOfMemory;
    }
    catch (...)
    {
        DEBUG_LOG("An exception was throwed while reading with vtkGDCMImageReader");
        WARN_LOG("An exception was throwed while reading with vtkGDCMImageReader");
        errorCode = UnknownError;
    }

    // Vtk no retorna cap error al fer un request d'abort i deixa les dades a mig fer, per tant, ho hem de marcar manualment.
    if (m_abortRequested)
    {
        errorCode = ReadAborted;
    }

    printDebugInfo();

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
    vtkImageData *imageData;
    //
    // Extret de gdcmviewer.cxx (gdcm\Utilities\VTK\Applications)
    //

    // In case of palette color, let's tell VTK to map color:
    // MONOCHROME1 is also implemented with a lookup table

    int imageFormat = m_vtkGDCMReader->GetImageFormat();
    if (imageFormat == VTK_LOOKUP_TABLE)
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
            DEBUG_LOG(">> Format RGB");
            imageColorMapper16->SetOutputFormatToRGB();
            imageColorMapper16->Update();
            imageData = imageColorMapper16->GetOutput();
            imageColorMapper16->Register(imageData);
            imageColorMapper16->Delete();
        }
        else
        {
            DEBUG_LOG("Mapejem una LUT de 8 bits");
            vtkImageMapToColors *imageColorMapper = vtkImageMapToColors::New();
            imageColorMapper->SetInput(m_vtkGDCMReader->GetOutput());
            imageColorMapper->SetLookupTable(m_vtkGDCMReader->GetOutput()->GetPointData()->GetScalars()->GetLookupTable());
            DEBUG_LOG(">> Format RGB");
            imageColorMapper->SetOutputFormatToRGB();
            imageColorMapper->Update();
            imageData = imageColorMapper->GetOutput();
            imageColorMapper->Register(imageData);
            imageColorMapper->Delete();
        }
    }
    else if (imageFormat == VTK_YBR)
    {
        DEBUG_LOG("Mapejem espai de color YBR a RGB");
        vtkImageYBRToRGB *ybrToRGBFilter = vtkImageYBRToRGB::New();
        ybrToRGBFilter->SetInput(m_vtkGDCMReader->GetOutput());
        ybrToRGBFilter->Update();
        imageData = ybrToRGBFilter->GetOutput();
        ybrToRGBFilter->Register(imageData);
        ybrToRGBFilter->Delete();
    }
    else if (imageFormat == VTK_RGB || imageFormat == VTK_RGBA)
    {
        DEBUG_LOG("Llegim directament de vtkGDCMImageReader. No cal aplicar cap mapeig adicional, les dades estan en format RGB/RGBA");
        // easy case !
        imageData = m_vtkGDCMReader->GetOutput();
    }
    else
    {
        DEBUG_LOG(QString("Llegim directament de vtkGDCMImageReader. No cal aplicar cap mapeig adicional. Format d'imatge: %1").arg(imageFormat));
        imageData = m_vtkGDCMReader->GetOutput();
    }

    m_volumePixelData = new VolumePixelData();
    m_volumePixelData->setData(imageData);
}

void VolumePixelDataReaderVTKGDCM::printDebugInfo()
{
    Q_ASSERT(m_vtkGDCMReader);

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

        case gdcm::PixelFormat::SINGLEBIT: // bool / monochrome
            DEBUG_LOG("SINGLE BIT");
            break;

        case gdcm::PixelFormat::UNKNOWN:
            DEBUG_LOG("UNKNOWN");
            break;

        default:
            DEBUG_LOG(QString("Scalar type not listed in switch: %1").arg(m_vtkGDCMReader->GetDataScalarType()));
            break;
    }
}

void VolumePixelDataReaderVTKGDCM::slotProgress()
{
    emit progress((int)(m_vtkGDCMReader->GetProgress() * 100));
}

} // End namespace udg
