/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGVOLUME_CPP
#define UDGVOLUME_CPP

#include "volume.h"

#include "volumereader.h"
#include "logging.h"
#include "image.h"
#include "series.h"
#include "study.h"
#include "patient.h"
#include "mathtools.h"
#include "coresettings.h"
// VTK
#include <vtkImageData.h>
// Voxel information
#include <vtkPointData.h>
#include <vtkCell.h>

namespace udg {

Volume::Volume(QObject *parent)
: QObject(parent)
{
    init();
}

Volume::Volume(ItkImageTypePointer itkImage, QObject *parent)
 : QObject(parent)
{
    init();
    this->setData(itkImage);
}

Volume::Volume(VtkImageTypePointer vtkImage, QObject *parent)
 : QObject(parent)
{
    init();
    this->setData(vtkImage);
}

void Volume::init()
{
    m_numberOfPhases = 1;
    m_numberOfSlicesPerPhase = 1;
    // TODO És millor crear un objecte o assignar-li NUL a l'inicialitzar? 
    // Així potser és més segur des del punt de vista de si li demanem propietats al volum com origen, espaiat, etc
    m_imageDataVTK = vtkImageData::New();

    // Preparem el lector de volums
    m_volumeReader = new VolumeReader();
    connect(m_volumeReader, SIGNAL(progress(int)), SIGNAL(progress(int)));
    
    m_itkToVtkFilter = ItkToVtkFilterType::New();
    m_vtkToItkFilter = VtkToItkFilterType::New();
    m_dataLoaded = false;
}

Volume::~Volume()
{
    delete m_volumeReader;
}

Volume::ItkImageTypePointer Volume::getItkData()
{
    m_vtkToItkFilter->SetInput(this->getVtkData());
    try
    {
        m_vtkToItkFilter->GetImporter()->Update();
    }
    catch (itk::ExceptionObject & excep)
    {
        WARN_LOG(QString("Excepció en el filtre vtkToItk :: Volume::getItkData() -> ") + excep.GetDescription());
    }
    return m_vtkToItkFilter->GetImporter()->GetOutput();
}

Volume::VtkImageTypePointer Volume::getVtkData()
{
    if (!m_dataLoaded)
    {
        m_volumeReader->read(this);
    }
    return m_imageDataVTK;
}

void Volume::setData(ItkImageTypePointer itkImage)
{
    m_itkToVtkFilter->SetInput(itkImage);
    try
    {
        m_itkToVtkFilter->Update();
    }
    catch (itk::ExceptionObject & excep)
    {
        WARN_LOG(QString("Excepció en el filtre itkToVtk :: Volume::setData(ItkImageTypePointer itkImage) -> ") + excep.GetDescription());
    }
    this->setData(m_itkToVtkFilter->GetOutput());
}

void Volume::setData(VtkImageTypePointer vtkImage)
{
    // TODO Fer còpia local, no només punter-> com fer-ho?
    if (m_imageDataVTK)
    {
        m_imageDataVTK->ReleaseData();
    }
    m_imageDataVTK = vtkImage;
    m_dataLoaded = true;
}

void Volume::getOrigin(double xyz[3])
{
    getVtkData()->UpdateInformation();
    getVtkData()->GetOrigin(xyz);
}

double* Volume::getOrigin()
{
    getVtkData()->UpdateInformation();
    return getVtkData()->GetOrigin();
}

void Volume::getSpacing(double xyz[3])
{
    getVtkData()->UpdateInformation();
    getVtkData()->GetSpacing(xyz);
}

double* Volume::getSpacing()
{
    getVtkData()->UpdateInformation();
    return getVtkData()->GetSpacing();
}

void Volume::getWholeExtent(int extent[6])
{
    getVtkData()->UpdateInformation();
    getVtkData()->GetWholeExtent(extent);
}

int* Volume::getWholeExtent()
{
    getVtkData()->UpdateInformation();
    return getVtkData()->GetWholeExtent();
}

int* Volume::getDimensions()
{
    getVtkData()->UpdateInformation();
    return getVtkData()->GetDimensions();
}

void Volume::getDimensions(int dims[3])
{
    getVtkData()->UpdateInformation();
    getVtkData()->GetDimensions(dims);
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
        // Obtenim el nombre d'imatges per fase
        int slices = getNumberOfSlicesPerPhase();
        int currentImageIndex = index;
        QList<Image *> phaseImages;
        for (int i = 0; i < slices; i++)
        {
            phaseImages << m_imageSet.at(currentImageIndex);
            currentImageIndex += m_numberOfPhases;
        }
        result->setImages(phaseImages);
    }
    return result;
}

QList<Image *> Volume::getPhaseImages(int index)
{
    QList<Image *> phaseImages;
    if (index >= 0 && index < m_numberOfPhases)
    {
        // Obtenim el nombre d'imatges per fase
        int slices = getNumberOfSlicesPerPhase();
        int currentImageIndex = index;
        for (int i = 0; i < slices; i++)
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
        m_dataLoaded = false;
    }
}

void Volume::setImages(const QList<Image *> &imageList)
{
    m_imageSet.clear();
    m_imageSet = imageList;
    m_dataLoaded = false;
}

QList<Image *> Volume::getImages() const
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

    if (m_dataLoaded)
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
        result = "The data is not loaded";
    }

    return result;
}

Image* Volume::getImage(int sliceNumber, int phaseNumber) const
{
    Image *image = NULL;

    if (!m_imageSet.isEmpty())
    {
        if ((sliceNumber*m_numberOfPhases + phaseNumber) < m_imageSet.count())
        {
            image = m_imageSet.at(sliceNumber*m_numberOfPhases + phaseNumber);
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
        const double *directionCosines = firstImage->getImageOrientationPatient();
        for (int i = 0; i < 3; i++)
        {
            direction[i] = directionCosines[i + 6];
        }
    }
    else
    {
        const double *firstOrigin = firstImage->getImagePositionPatient();
        const double *secondOrigin = secondImage->getImagePositionPatient();
        // calculem la direcció real de com estan apilades
        double *zDirection = MathTools::directorVector(firstOrigin, secondOrigin);
        MathTools::normalize(zDirection);
        for (int i = 0; i < 3; i++)
        {
            direction[i] = zDirection[i];
        }
    }
}

Volume::VoxelType* Volume::getScalarPointer(int x, int y, int z)
{
	// TODO Caldria posar static/dynamic_cast? o en aquest cas ja és suficient així?
	return (Volume::VoxelType *)this->getVtkData()->GetScalarPointer(x,y,z);
}

Volume::VoxelType* Volume::getScalarPointer(int index[3])
{
	// TODO Caldria posar static/dynamic_cast? o en aquest cas ja és suficient així?
	return this->getScalarPointer(index[0], index[1], index[2]);
}

bool Volume::getVoxelValue(double coordinate[3], Volume::VoxelType &voxelValue)
{
    vtkImageData *vtkData = getVtkData();
    if (!vtkData)
    {
        DEBUG_LOG("Dades VTK nul·les!");
        return false;
    }
    
    // Use tolerance as a function of size of source data
    double tolerance = vtkData->GetLength();
    tolerance = tolerance ? tolerance*tolerance / 1000.0 : 0.001;

    int subCellId;
    double parametricCoordinates[3], interpolationWeights[8];
    bool found = false;

    // Find the cell that contains q and get it
    vtkCell *cell = vtkData->FindAndGetCell(coordinate, NULL, -1, tolerance, subCellId, parametricCoordinates, interpolationWeights);
    if (cell)
    {
        vtkPointData *pointData = vtkData->GetPointData();
        vtkPointData *outPointData = vtkPointData::New();
        outPointData->InterpolateAllocate(pointData, 1, 1);
        // Interpolate the point data
        outPointData->InterpolatePoint(pointData, 0, cell->PointIds, interpolationWeights);
        voxelValue = outPointData->GetScalars()->GetTuple1(0);
        found = true;
        outPointData->Delete();
    }

    return found;
}

void Volume::createNeutralVolume()
{
    if (m_imageDataVTK)
        m_imageDataVTK->Delete();
    // Creem un objecte vtkImageData "neutre"
    m_imageDataVTK = vtkImageData::New();
    // Inicialitzem les dades
    m_imageDataVTK->SetOrigin(.0, .0, .0);
    m_imageDataVTK->SetSpacing(1., 1., 1.);
    m_imageDataVTK->SetDimensions(10, 10, 1);
    m_imageDataVTK->SetWholeExtent(0, 9, 0, 9, 0, 0);
    m_imageDataVTK->SetScalarTypeToShort();
    m_imageDataVTK->SetNumberOfScalarComponents(1);
    m_imageDataVTK->AllocateScalars();
    // Omplim el dataset perquè la imatge resultant quedi amb un cert degradat
    signed short * scalarPointer = (signed short *) m_imageDataVTK->GetScalarPointer();
    signed short value;
    for (int i = 0; i < 10; i++)
    {
        value = 150-i*20;
        if (i > 4)
        {
            value = 150 - (10 - i - 1)*20;
        }

        for (int j = 0; j < 10; j++)
        {            
            *scalarPointer = value;
            *scalarPointer++;
        }
    }
    // Quan creem el volum neutre indiquem que només tenim 1 sola fase 
    // TODO Potser s'haurien de crear tantes fases com les que indiqui la sèrie?
    this->setNumberOfPhases(1);
    
    // Indiquem que hem carregat les dades
    m_dataLoaded = true;
}

bool Volume::fitsIntoMemory()
{
    if (m_dataLoaded)
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
    catch (std::bad_alloc &ba)
    {
        return false;
    }
}

};

#endif
