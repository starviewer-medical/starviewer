#include "volumereader.h"

#include "volumepixeldatareaderitkdcmtk.h"
#include "volumepixeldatareaderitkgdcm.h"
#include "volumepixeldatareadervtkgdcm.h"

#include "volume.h"
#include "image.h"
#include "series.h"
#include "logging.h"
#include "starviewerapplication.h"
#include "coresettings.h"
#include "computezspacingpostprocessor.h"

#include <QMessageBox>

namespace udg {

VolumeReader::VolumeReader(QObject *parent)
    : QObject(parent), m_volumePixelDataReader(0)
{
     m_lastError = VolumePixelDataReader::NoError;
}

VolumeReader::~VolumeReader()
{
    if (m_volumePixelDataReader)
    {
        delete m_volumePixelDataReader;
    }
}

void VolumeReader::executePixelDataReader(Volume *volume)
{
    if (!volume)
    {
        DEBUG_LOG("El volum proporcionat és NUL! Retornem.");
        return;
    }

    m_lastError = VolumePixelDataReader::NoError;

    // Obtenim els arxius que hem de llegir
    QStringList fileList = this->getFilesToRead(volume);
    if (!fileList.isEmpty())
    {
        // Per evitar donar informació poc acurada, només informarem del progrés quan tinguem més d'una imatge si no, amb imatges úniques molt grans
        // o multiframes passaríem de 0 a 100 quedant a 0 molta estona.
        bool showProgress = fileList.count() > 1;

        // Posem a punt el reader i llegim les dades
        PixelDataReaderType readerType = this->getSuitableReader(volume);
        this->setUpReader(readerType, showProgress);
        this->setUpPostprocessors(readerType);

        m_lastError = m_volumePixelDataReader->read(fileList);
        if (m_lastError == VolumePixelDataReader::NoError)
        {
            // Tot ha anat ok, assignem les dades al volum
            volume->setPixelData(m_volumePixelDataReader->getVolumePixelData());
            runPostprocessors(volume);
            fixSpacingIssues(volume);
        }
        else
        {
            volume->convertToNeutralVolume();
            this->logWarningLastError(fileList);
        }
    }
}

void VolumeReader::read(Volume *volume)
{
    this->executePixelDataReader(volume);
    this->showMessageBoxWithLastError();
}

bool VolumeReader::readWithoutShowingError(Volume *volume)
{
    this->executePixelDataReader(volume);

    return m_lastError == VolumePixelDataReader::NoError;
}

void VolumeReader::requestAbort()
{
    m_volumePixelDataReader->requestAbort();
}

void VolumeReader::showMessageBoxWithLastError() const
{
    if (m_lastError == VolumePixelDataReader::NoError)
    {
        return;
    }

    QString messageBoxTitle = "";
    switch (m_lastError)
    {
        case VolumePixelDataReader::OutOfMemory:
            messageBoxTitle = tr("Out of memory");
            break;

        case VolumePixelDataReader::MissingFile:
            messageBoxTitle = tr("Missing Files");
            break;

        case VolumePixelDataReader::CannotReadFile:
            messageBoxTitle = tr("Cannot Read File(s)");
            break;

        case VolumePixelDataReader::UnknownError:
            messageBoxTitle = tr("Unknown Error");
            break;
    }

    QMessageBox::warning(0, messageBoxTitle, this->getLastErrorMessageToUser());
}

QString VolumeReader::getLastErrorMessageToUser() const
{
    switch (m_lastError)
    {
        case VolumePixelDataReader::OutOfMemory:
            return tr("There's not enough memory to load the Series you requested. Try to close all the open %1 windows and restart "
                      "the application and try again. If the problem persists, adding more RAM memory or switching to a 64-bit operating "
                      "system may solve the problem.").arg(ApplicationNameString);
        case VolumePixelDataReader::MissingFile:
            return tr("%1 could not find the corresponding files for this Series. Maybe they had been removed or are corrupted.").arg(ApplicationNameString);
        case VolumePixelDataReader::CannotReadFile:
            return tr("%1 could not read the requested file(s) for this Series.").arg(ApplicationNameString);
        case VolumePixelDataReader::ReadAborted:
            return tr("Loading of data has been aborted.");
        case VolumePixelDataReader::UnknownError:
            return tr("%1 found an unexpected error reading this Series. No Series data has been loaded.").arg(ApplicationNameString);
        case VolumePixelDataReader::NoError:
            return "";
    }
    return "";
}

void VolumeReader::logWarningLastError(const QStringList &fileList) const
{
    switch (m_lastError)
    {
        case VolumePixelDataReader::OutOfMemory:
            WARN_LOG("No podem carregar els arxius perquè no caben a memòria\n" + fileList.join("\n"));
            break;

        case VolumePixelDataReader::MissingFile:
            WARN_LOG("No podem carregar els arxius perquè falten fitxers");
            break;

        case VolumePixelDataReader::CannotReadFile:
            WARN_LOG("No podem llegir els següents arxius (raó desconeguda)\n" + fileList.join("\n"));
            break;

        case VolumePixelDataReader::UnknownError:
            WARN_LOG("No podem carregar els arxius perquè ha donat un error desconegut");
            break;
    }
}

void VolumeReader::fixSpacingIssues(Volume *volume)
{
    if (!volume)
    {
        DEBUG_LOG("El volum és nul!");
        return;
    }

    if (volume->getNumberOfPhases() > 1 && volume->getNumberOfSlicesPerPhase() > 1)
    {
        double zSpacing = abs(Image::distance(volume->getImage(0)) - Image::distance(volume->getImage(1)));
        DEBUG_LOG(QString("Arreglem el z-spacing per volum amb fases. z-spacing llegit (mal calculat): %1  - Nou z-spacing ben calculat: %2")
            .arg(volume->getSpacing()[2]).arg(zSpacing));
        
        volume->getSpacing()[2] = zSpacing;
    }
}

QStringList VolumeReader::getFilesToRead(Volume *volume) const
{
    QStringList fileList;
    foreach (Image *image, volume->getImages())
    {
        // Evitem afegir més vegades l'arxiu si aquest és multiframe
        if (!fileList.contains(image->getPath()))
        {
            fileList << image->getPath();
        }
    }

    return fileList;
}

VolumeReader::PixelDataReaderType VolumeReader::getSuitableReader(Volume *volume) const
{
    // Primer de tot comprovarem si hem de forçar la lectura amb alguna llibreria específica
    // Això només es farà servir com a backdoor en casos molt específics i controlats per poder
    // oferir un workaround fins que puguem oferir una bugfix release que arregli el problema correctament
    PixelDataReaderType forcedReaderLibrary;
    if (mustForceReaderLibraryBackdoor(volume, forcedReaderLibrary))
    {
        return forcedReaderLibrary;
    }

    int firstImageRows = 0;
    int firstImageColumns = 0;
    QList<Image*> imageSet = volume->getImages();
    if (!imageSet.empty())
    {
        firstImageRows = imageSet.first()->getRows();
        firstImageColumns = imageSet.first()->getColumns();
    }

    bool containsDifferentSizeImages = false;
    bool containsColorImages = false;
    bool avoidWrongPixelType = false;

    foreach (Image *image, imageSet)
    {
        // Comprovem que no tingui imatges de diferents mides
        // TODO Aquesta comprovació podria desaparèixer ja que ara fem que les
        // imatges de diferents mides es guardin en volums diferents, per tant, hauria de
        // ser anòmal i inesperat trobar-nos amb aquest cas. Tot i així ens serveix per evitar que
        // si es donés el cas ens petés el programa
        if (firstImageRows != image->getRows() || firstImageColumns != image->getColumns())
        {
            DEBUG_LOG("Tenim imatges de diferents mides!");
            containsDifferentSizeImages = true;
        }

        // Comprovem si es tracta d'una imatge a color
        // TODO Caldria diferenciar també els casos en que tenim més d'una imatge de diferents mides i que alguna és de color
        QString photometricInterpretation = image->getPhotometricInterpretation();
        if (!photometricInterpretation.contains("MONOCHROME"))
        {
            // Si photometric interpretation no és ni MONOCHROME1 ni MONOCHROME2, llavors és algun tipu d'imatge a color:
            // PALETTE COLOR, RGB YBR_FULL, YBR_FULL_422, YBR_PARTIAL_422, YBR_PARTIAL_420, YBR_ICT o YBR_RCT
            containsColorImages = true;
            DEBUG_LOG("Photometric Interpretation: " + photometricInterpretation);
        }
        else if (image->getBitsAllocated() == 16 && image->getBitsStored() == 16 && !image->getSOPInstanceUID().contains("MHDImage"))
        {
            // Aquesta comprovació es fa per evitar casos com el del ticket #1257
            // Com que itkImage sempre s'allotja amb el tipus de pixel signed short int,
            // quan tenim 16 bits allocated i stored, podria ser que el rang de dades necessités
            // que el tipus de pixel fos unsigned short int (0..65536) perquè la imatge es visualitzi correctament

            // Només ho aplicarem si es tracta de modalitats d'imatge "no volumètriques" per acotar el problema
            // i evitar que es produeixi el problema remarcat al ticket #1313
            QStringList supportedModalities;
            supportedModalities << "CR" << "RF" << "DX" << "MG" << "OP" << "US" << "ES" << "NM" << "DT" << "PT" << "XA" << "XC";
            if (supportedModalities.contains(image->getParentSeries()->getModality()))
            {
                avoidWrongPixelType = true;
            }
        }
    }

    if (!containsDifferentSizeImages && containsColorImages)
    {
        // Si conté imatges de color i totes són de la mateixa mida les llegirem amb VTK-GDCM
        return VTKGDCMPixelDataReader;
    }
    else if (avoidWrongPixelType)
    {
        // Com que el reader de vtkGDCM decideix el tipus dinàmicament, allotjarem el tipus de pixel correcte
        return VTKGDCMPixelDataReader;
    }
    else if (volume->isMultiframe())
    {
        // Si és un volum multiframe el llegirem amb ITK-DCMTK per evitar pics de memòria que es produeixen amb GDCM
        return ITKDCMTKPixelDataReader;
    }
    else
    {
        // TODO De moment, per defecte llegirem amb ITK-GDCM excepte en les condicions anteriors
        return ITKGDCMPixelDataReader;
    }
}

bool VolumeReader::mustForceReaderLibraryBackdoor(Volume *volume, PixelDataReaderType &forcedReaderLibrary) const
{
    Q_ASSERT(volume);

    bool forceLibrary = false;
    Settings settings;

    // Primer comprovem el setting més prioritari, que indicaria que tot s'ha de llegir amb una única llibreria
    QString forceReadingWithSpecfiedLibrary = settings.getValue(CoreSettings::ForcedImageReaderLibrary).toString().trimmed();
    if (forceReadingWithSpecfiedLibrary == "vtk")
    {
        INFO_LOG("Forcem la lectura de qualsevol imatge amb vtk");
        forcedReaderLibrary = VTKGDCMPixelDataReader;
        forceLibrary = true;
    }
    else if (forceReadingWithSpecfiedLibrary == "itk")
    {
        INFO_LOG("Forcem la lectura de qualsevol imatge amb itk");
        forcedReaderLibrary = ITKGDCMPixelDataReader;
        forceLibrary = true;
    }
    else if (forceReadingWithSpecfiedLibrary == "itkdcmtk")
    {
        INFO_LOG("Forcem la lectura de qualsevol imatge amb itkdcmtk");
        forcedReaderLibrary = ITKDCMTKPixelDataReader;
        forceLibrary = true;
    }

    // Si l'anterior no està assignat comprovarem el següent setting, que ens indica la llibreria segons la modalitat de la imatge
    if (!forceLibrary)
    {
        // Obtenim la modalitat del volum actual
        QString modality;
        Image *firstImage = volume->getImage(0);
        if (firstImage)
        {
            modality = firstImage->getParentSeries()->getModality();
        }

        // Primer comprovem si hi ha modalitats a forçar per ITK
        QStringList forceITKForModalities = settings.getValue(CoreSettings::ForceITKImageReaderForSpecifiedModalities).toString().trimmed().split("\\");
        if (forceITKForModalities.contains(modality))
        {
            INFO_LOG("Forcem la lectura del volum actual amb ITK perquè és de modalitat=" + modality);
            forcedReaderLibrary = ITKGDCMPixelDataReader;
            forceLibrary = true;
        }

        // Si no s'han de forçar per ITK ho comprovem per VTK
        if (!forceLibrary)
        {
            QStringList forceVTKForModalities = settings.getValue(CoreSettings::ForceVTKImageReaderForSpecifiedModalities).toString().trimmed().split("\\");
            if (forceVTKForModalities.contains(modality))
            {
                INFO_LOG("Forcem la lectura del volum actual amb VTK perquè és de modalitat=" + modality);
                forcedReaderLibrary = VTKGDCMPixelDataReader;
                forceLibrary = true;
            }
        }
    }

    return forceLibrary;
}

void VolumeReader::setUpReader(PixelDataReaderType readerType, bool showProgress)
{
    // Eliminem un lector anterior si l'havia
    if (m_volumePixelDataReader)
    {
        delete m_volumePixelDataReader;
    }

    // Segons quin pixel data reader estigui seleccionat, crearem l'objecte que toqui
    switch (readerType)
    {
        case ITKDCMTKPixelDataReader:
            m_volumePixelDataReader = new VolumePixelDataReaderITKDCMTK(this);
            DEBUG_LOG("Escollim ITK-DCMTK per llegir la pixel data del volum");
            break;

        case ITKGDCMPixelDataReader:
            m_volumePixelDataReader = new VolumePixelDataReaderITKGDCM(this);
            DEBUG_LOG("Escollim ITK-GDCM per llegir la pixel data del volum");
            break;

        case VTKGDCMPixelDataReader:
            m_volumePixelDataReader = new VolumePixelDataReaderVTKGDCM(this);
            DEBUG_LOG("Escollim VTK-GDCM per llegir la pixel data del volum");
            break;
    }

    if (showProgress)
    {
        // Connectem les senyals de notificació de progrés
        connect(m_volumePixelDataReader, SIGNAL(progress(int)), SIGNAL(progress(int)));
    }
}

void VolumeReader::setUpPostprocessors(PixelDataReaderType readerType)
{
    m_postprocessorsQueue.clear();

    switch (readerType)
    {
        case ITKDCMTKPixelDataReader:
            m_postprocessorsQueue.enqueue(QSharedPointer<Postprocessor>(new ComputeZSpacingPostprocessor()));
            break;

        default:
            break;
    }
}

void VolumeReader::runPostprocessors(Volume *volume)
{
    while (!m_postprocessorsQueue.isEmpty())
    {
        m_postprocessorsQueue.dequeue()->postprocess(volume);
    }
}

} // End namespace udg
