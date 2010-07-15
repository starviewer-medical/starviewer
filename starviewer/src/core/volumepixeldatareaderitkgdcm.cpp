#include "volumepixeldatareaderitkgdcm.h"

#include "logging.h"
#include "dicomtagreader.h"

// ITK
#include <itkTileImageFilter.h>
// Qt
#include <QFileInfo>
#include <QDir>

namespace udg {

VolumePixelDataReaderITKGDCM::VolumePixelDataReaderITKGDCM(QObject *parent)
: VolumePixelDataReader(parent)
{
    m_reader = ReaderType::New();
    m_seriesReader = SeriesReaderType::New();

    m_gdcmIO = ImageIOType::New();

    m_itkToVtkFilter = ItkToVtkFilterType::New();
    
    m_progressSignalAdaptor = new itk::QtSignalAdaptor();
    // Connect the adaptor as an observer of a Filter's event
    m_seriesReader->AddObserver(itk::ProgressEvent(), m_progressSignalAdaptor->GetCommand());

    // Connect the adaptor's Signal to the Qt Widget Slot
    connect(m_progressSignalAdaptor, SIGNAL(Signal()), SLOT(slotProgress()));
}

VolumePixelDataReaderITKGDCM::~VolumePixelDataReaderITKGDCM()
{
}

int VolumePixelDataReaderITKGDCM::read(const QStringList &filenames)
{
    int errorCode = NoError;
    
    if (filenames.isEmpty())
    {
        WARN_LOG("La llista de noms de fitxer per carregar és buida");
        errorCode = InvalidFileName;
    }
    else
    {
        if (filenames.size() > 1)
        {
            errorCode = readMultipleFiles(filenames);
        }
        else
        {
            errorCode = readSingleFile(filenames.first());
        }
    }
    
    return errorCode;
}

int VolumePixelDataReaderITKGDCM::readMultipleFiles(const QStringList &filenames)
{
    // Convertim la QStringList al format std::vector<std::string> que s'esperen les itk
    std::vector<std::string> stlFilenames;
    for (int i = 0; i < filenames.size(); i++)
    {
        stlFilenames.push_back(filenames.at(i).toStdString());
    }
    m_seriesReader->SetFileNames(stlFilenames);

    int errorCode = NoError;
    try
    {
        m_seriesReader->Update();
    }
    catch (itk::ExceptionObject & e)
    {
        WARN_LOG(QString("Excepció llegint els arxius del directori [%1] Descripció: [%2]").arg(QFileInfo(filenames.at(0)).dir().path()).arg(e.GetDescription()));
        // Llegim el missatge d'error per esbrinar de quin error es tracta
        errorCode = identifyErrorMessage(QString(e.GetDescription()) );
    }
    
    switch (errorCode)
    {
        case NoError:
            setData(m_seriesReader->GetOutput());
            break;

        case SizeMismatch:
            errorCode = NoError;
            readDifferentSizeImagesIntoOneVolume(filenames);
            break;

        case ZeroSpacingNotAllowed:
            errorCode = NoError;
            // Assignem les dades llegides, aquesta excepció simplement és una mena de warning. 
            // En el cas del z-spacing 0 es pot deure a que la informació estigui "amagada" en una seqüència privada
            // o que realment la imatge en sí només té sentit com a 2D i no 3D
            setData(m_seriesReader->GetOutput());
            checkZeroSpacingException();
            break;
    }

    return errorCode;
}

int VolumePixelDataReaderITKGDCM::readSingleFile(const QString &fileName)
{
    int errorCode = NoError;

    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName(qPrintable(fileName));
    emit progress(0);
    try
    {
        reader->Update();
    }
    catch (itk::ExceptionObject & e)
    {
        WARN_LOG(QString("Excepció llegint l'arxiu [%1] Descripció: [%2]").arg(fileName).arg(e.GetDescription()));
        // Llegim el missatge d'error per esbrinar de quin error es tracta
        errorCode = identifyErrorMessage(QString(e.GetDescription()));
    }
    catch (std::bad_alloc)
    {
        errorCode = OutOfMemory;
    }

    switch (errorCode)
    {
        case NoError:
            setData(reader->GetOutput());
            break;
    
        case ZeroSpacingNotAllowed:
            errorCode = NoError;
            // Assignem les dades llegides, aquesta excepció simplement és una mena de warning. 
            // En el cas del z-spacing 0 es pot deure a que la informació estigui "amagada" en una seqüència privada
            // o que realment la imatge en sí només té sentit com a 2D i no 3D
            setData(reader->GetOutput());
            checkZeroSpacingException();
            break;
    }

    emit progress(100);

    return errorCode;
}

void VolumePixelDataReaderITKGDCM::readDifferentSizeImagesIntoOneVolume(const QStringList &filenames)
{
    int errorCode = NoError;
    // Declarem el filtre de tiling
    typedef itk::TileImageFilter<Volume::ItkImageType, Volume::ItkImageType> TileFilterType;
    TileFilterType::Pointer tileFilter = TileFilterType::New();
    // Inicialitzem les seves variables
    // El layout ens serveix per indicar cap on creix la cua. En aquest cas volem fer creixer la coordenada Z
    TileFilterType::LayoutArrayType layout;
    layout[0] = 1;
    layout[1] = 1;
    layout[2] = 0;
    tileFilter->SetLayout(layout);

    int progressCount = 0;
    int progressIncrement = static_cast<int>((1.0 / (double)filenames.count()) * 100);

    m_reader->SetImageIO(m_gdcmIO);
    foreach (const QString &file, filenames)
    {
        emit progress(progressCount);
        // Declarem la imatge que volem carregar
        Volume::ItkImageTypePointer itkImage;
        m_reader->SetFileName(qPrintable(file));
        try
        {
            m_reader->UpdateLargestPossibleRegion();
        }
        catch (itk::ExceptionObject & e)
        {
            WARN_LOG(QString("Excepció llegint els arxius del directori [%1] Descripció: [%2]").arg(QFileInfo(filenames.at(0)).dir().path()).arg(e.GetDescription()));

            // Llegim el missatge d'error per esbrinar de quin error es tracta
            errorCode = identifyErrorMessage(QString(e.GetDescription()));
        }
        if (errorCode == NoError)
        {
            itkImage = m_reader->GetOutput();
            m_reader->GetOutput()->DisconnectPipeline();
        }
        // TODO No es fa tractament d'errors!

        // Un cop llegit el block, fem el tiling
        tileFilter->PushBackInput(itkImage);
        progressCount += progressIncrement;
    }
    tileFilter->Update();
    setData(tileFilter->GetOutput());
    emit progress(100);
}

int VolumePixelDataReaderITKGDCM::identifyErrorMessage(const QString &errorMessage)
{
    if (errorMessage.contains("Size mismatch") || errorMessage.contains("ImageIO returns IO region that does not fully contain the requested regionRequested"))
    {
        return SizeMismatch;
    }
    else if (errorMessage.contains("Failed to allocate memory for image"))
    {
        return OutOfMemory;
    }
    else if (errorMessage.contains("The file doesn't exists"))
    {
        return MissingFile;
    }
    else if (errorMessage.contains("A spacing of 0 is not allowed"))
    {
        return ZeroSpacingNotAllowed;
    }
    else
    {
        return UnknownError;
    }
}

void VolumePixelDataReaderITKGDCM::setData(Volume::ItkImageTypePointer itkImage)
{
    // Li donem l'input al filtre i fem la conversió
    m_itkToVtkFilter->SetInput(itkImage);
    try
    {
        m_itkToVtkFilter->Update();
    }
    catch (itk::ExceptionObject & exception)
    {
        WARN_LOG(QString("Excepció en el filtre itkToVtk :: VolumePixelDataReaderITKGDCM::setData(ItkImageTypePointer itkImage) -> ") + exception.GetDescription() );
    }
    // Assignem l'output
    m_vtkImageData = m_itkToVtkFilter->GetOutput();
}

void VolumePixelDataReaderITKGDCM::checkZeroSpacingException()
{
    if (m_vtkImageData)
    {
        double spacing[3];

        m_vtkImageData->GetSpacing(spacing);
        DEBUG_LOG(QString("checkZeroSpacing: (x , y , z) = (%1 , %2 , %3)").arg(spacing[0]).arg(spacing[1]).arg(spacing[2]));

        if (spacing[0] == 0.0 || spacing[1] == 0.0)
        {
            WARN_LOG(QString("x ó y spacing és 0; [x,y] = [%1,%2]. Donem el volum per vàlid igualment.").arg(spacing[0]).arg(spacing[1]));
        }
        else if (spacing[2] == 0.0)
        {
            WARN_LOG("El z-spacing de les dades llegides és 0. Possiblement la informació corresponent (SliceThikness/SpacingBetweenSlices) estigui dins de seqüències privades. Donem el volum per vàlid igualment.");
        }
    }
    else
    {
        DEBUG_LOG("No s'han assignat les dades vtk! No podem fer cap comprovació.");
    }
}

void VolumePixelDataReaderITKGDCM::slotProgress()
{
    emit progress((int)(m_seriesReader->GetProgress()*100));
}

} // End namespace udg
