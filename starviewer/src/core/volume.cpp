#ifndef UDGVOLUME_CPP
#define UDGVOLUME_CPP

#include "volume.h"

#include "volumereader.h"
#include "logging.h"
#include "image.h"
#include "series.h"
#include "study.h"
#include "mathtools.h"

namespace udg {

Volume::Volume(QObject *parent)
: QObject(parent)
{
    m_numberOfPhases = 1;
    m_numberOfSlicesPerPhase = 1;

    m_volumePixelDataLoaded = false;
    m_volumePixelData = new VolumePixelData(this);
}

Volume::~Volume()
{
    DEBUG_LOG(QString("Destructor ~Volume %1, name: %2").arg(m_identifier.getValue()).arg(this->objectName()));
    delete m_volumePixelData;
}

Volume::ItkImageTypePointer Volume::getItkData()
{
    return this->getPixelData()->getItkData();
}

vtkImageData* Volume::getVtkData()
{
    return this->getPixelData()->getVtkData();
}

void Volume::setData(ItkImageTypePointer itkImage)
{
    m_volumePixelData->setData(itkImage);
    m_volumePixelDataLoaded = true;
}

void Volume::setData(vtkImageData *vtkImage)
{
    m_volumePixelData->setData(vtkImage);
    m_volumePixelDataLoaded = true;
}

void Volume::setPixelData(VolumePixelData *pixelData)
{
    m_volumePixelData = pixelData;
    m_volumePixelDataLoaded = true;
}

VolumePixelData* Volume::getPixelData()
{
    if (!m_volumePixelDataLoaded)
    {
        VolumeReader *volumeReader = new VolumeReader();
        connect(volumeReader, SIGNAL(progress(int)), SIGNAL(progress(int)));
        volumeReader->read(this);
        delete volumeReader;
    }

    return m_volumePixelData;
}

bool Volume::isPixelDataLoaded() const
{
    return m_volumePixelDataLoaded;
}

void Volume::getOrigin(double xyz[3])
{
    getVtkData()->GetOrigin(xyz);
}

double* Volume::getOrigin()
{
    return getVtkData()->GetOrigin();
}

void Volume::getSpacing(double xyz[3])
{
    getVtkData()->GetSpacing(xyz);
}

double* Volume::getSpacing()
{
    return getVtkData()->GetSpacing();
}

void Volume::getWholeExtent(int extent[6])
{
    getVtkData()->GetWholeExtent(extent);
}

int* Volume::getWholeExtent()
{
    return getVtkData()->GetWholeExtent();
}

int* Volume::getDimensions()
{
    return getVtkData()->GetDimensions();
}

void Volume::getDimensions(int dims[3])
{
    getVtkData()->GetDimensions(dims);
}

void Volume::getScalarRange(double range[2])
{
    getVtkData()->GetScalarRange(range);
}

void Volume::setIdentifier(const Identifier &id)
{
    m_identifier = id;
}

Identifier Volume::getIdentifier() const
{
    return m_identifier;
}

void Volume::setThumbnail(const QPixmap &thumbnail)
{
    m_thumbnail = thumbnail;
}

QPixmap Volume::getThumbnail() const
{
    return m_thumbnail;
}

void Volume::setNumberOfPhases(int phases)
{
    if (phases >= 1)
    {
        m_numberOfPhases = phases;
    }
}

int Volume::getNumberOfPhases() const
{
    return m_numberOfPhases;
}

Volume* Volume::getPhaseVolume(int index)
{
    Volume *result = NULL;
    if (m_numberOfPhases == 1)
    {
        // Si només tenim una sola fase, retornem totes les imatges que conté el volum
        result = new Volume();
        result->setImages(m_imageSet);
    }
    else if (index >= 0 && index < m_numberOfPhases)
    {
        result = new Volume();
        result->setImages(this->getPhaseImages(index));
    }
    return result;
}

QList<Image*> Volume::getPhaseImages(int index)
{
    QList<Image*> phaseImages;
    if (index >= 0 && index < m_numberOfPhases)
    {
        std::cout<<"getphaseimages"<<std::endl;

        // Obtenim el nombre d'imatges per fase
        int slices = getNumberOfSlicesPerPhase();
        int currentImageIndex = index;
        for (int i = 0; i < slices; ++i)
        {
            phaseImages << m_imageSet.at(currentImageIndex);
            currentImageIndex += m_numberOfPhases;
        }
    }
    return phaseImages;
}

void Volume::setNumberOfSlicesPerPhase(int slicesPerPhase)
{
    m_numberOfSlicesPerPhase = slicesPerPhase;
}

int Volume::getNumberOfSlicesPerPhase() const
{
    return m_numberOfSlicesPerPhase;
}

void Volume::addImage(Image *image)
{
    if (!m_imageSet.contains(image))
    {
        m_imageSet << image;
        m_volumePixelDataLoaded = false;
    }
}

void Volume::setImages(const QList<Image*> &imageList)
{
    m_imageSet.clear();
    m_imageSet = imageList;
    m_volumePixelDataLoaded = false;
}

QList<Image*> Volume::getImages() const
{
    return m_imageSet;
}

int Volume::getNumberOfFrames() const
{
    return m_imageSet.count();
}

Study* Volume::getStudy()
{
    if (!m_imageSet.isEmpty())
    {
        return m_imageSet.at(0)->getParentSeries()->getParentStudy();
    }
    else
    {
        return NULL;
    }
}

Patient* Volume::getPatient()
{
    if (this->getStudy())
    {
        return this->getStudy()->getParentPatient();
    }
    else
    {
        return NULL;
    }
}

QString Volume::toString(bool verbose)
{
    Q_UNUSED(verbose);
    QString result;

    if (m_volumePixelDataLoaded)
    {
        int dims[3];
        double origin[3];
        double spacing[3];
        int extent[6];
        double bounds[6];

        this->getDimensions(dims);
        this->getOrigin(origin);
        this->getSpacing(spacing);
        this->getWholeExtent(extent);
        this->getVtkData()->GetBounds(bounds);

        result += QString("Dimensions: %1, %2, %3").arg(dims[0]).arg(dims[1]).arg(dims[2]);
        result += QString("\nOrigin: %1, %2, %3").arg(origin[0]).arg(origin[1]).arg(origin[2]);
        result += QString("\nSpacing: %1, %2, %3").arg(spacing[0]).arg(spacing[1]).arg(spacing[2]);
        result += QString("\nExtent: %1..%2, %3..%4, %5..%6").arg(extent[0]).arg(extent[1]).arg(extent[2]).arg(extent[3]).arg(extent[4]).arg(extent[5]);
        result += QString("\nBounds: %1..%2, %3..%4, %5..%6").arg(bounds[0]).arg(bounds[1]).arg(bounds[2]).arg(bounds[3]).arg(bounds[4]).arg(bounds[5]);
    }
    else
    {
        result = "Data are not loaded yet";
    }

    return result;
}

Image* Volume::getImage(int sliceNumber, int phaseNumber) const
{
    Image *image = NULL;

    if (!m_imageSet.isEmpty())
    {
        int imageIndex = getImageIndex(sliceNumber, phaseNumber);
        if (imageIndex >= 0 && imageIndex < m_imageSet.count())
        {
            image = m_imageSet.at(imageIndex);
        }
    }

    return image;
}

void Volume::getStackDirection(double direction[3], int stack)
{
    // TODO Encara no suportem múltiples stacks!!!!
    // Fem el tractament com si només hi hagués un sol
    Q_UNUSED(stack);
    Image *firstImage = this->getImage(0);
    Image *secondImage = this->getImage(1);
    if (!firstImage)
    {
        DEBUG_LOG("Error gravísim. No hi ha 'primera' imatge!");
        return;
    }

    if (!secondImage)
    {
        DEBUG_LOG("Només hi ha una imatge per stack! Retornem la normal del pla");
        QVector3D normalVector = firstImage->getImageOrientationPatient().getNormalVector();
        direction[0] = normalVector.x();
        direction[1] = normalVector.y();
        direction[2] = normalVector.z();
    }
    else
    {
        const double *firstOrigin = firstImage->getImagePositionPatient();
        const double *secondOrigin = secondImage->getImagePositionPatient();
        // Calculem la direcció real de com estan apilades
        QVector3D zDirection = MathTools::directorVector(QVector3D(firstOrigin[0], firstOrigin[1], firstOrigin[2]),
                                                         QVector3D(secondOrigin[0], secondOrigin[1], secondOrigin[2]));
        zDirection.normalize();
        direction[0] = zDirection.x();
        direction[1] = zDirection.y();
        direction[2] = zDirection.z();
    }
}

Volume::VoxelType* Volume::getScalarPointer(int x, int y, int z)
{
    return this->getPixelData()->getScalarPointer(x, y, z);
}

Volume::VoxelType* Volume::getScalarPointer(int index[3])
{
    return this->getScalarPointer(index[0], index[1], index[2]);
}

void Volume::convertToNeutralVolume()
{
    m_volumePixelData->convertToNeutralPixelData();

    // Quan creem el volum neutre indiquem que només tenim 1 sola fase
    // TODO Potser s'haurien de crear tantes fases com les que indiqui la sèrie?
    this->setNumberOfPhases(1);

    // Indiquem que hem carregat les dades
    m_volumePixelDataLoaded = true;
}

AnatomicalPlane::AnatomicalPlaneType Volume::getAcquisitionPlane() const
{
    if (m_imageSet.isEmpty())
    {
        return AnatomicalPlane::NotAvailable;
    }
    else
    {
        return AnatomicalPlane::getPlaneTypeFromPatientOrientation(m_imageSet.first()->getPatientOrientation());
    }
}

int Volume::getImageIndex(int sliceNumber, int phaseNumber) const
{
    return sliceNumber * m_numberOfPhases + phaseNumber;
}

bool Volume::fitsIntoMemory()
{
    if (m_volumePixelDataLoaded)
    {
        return true;
    }

    unsigned long long int size = 0;
    foreach (Image *image, m_imageSet)
    {
        size += image->getColumns() * image->getRows() * sizeof(VoxelType);
    }

    char *p = 0;
    try
    {
        p = new char[size];
        delete[] p;
        return true;
    }
    catch (std::bad_alloc)
    {
        return false;
    }
}

};

#endif
