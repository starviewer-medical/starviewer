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

#include "volume.h"

#include "volumereader.h"
#include "logging.h"
#include "image.h"
#include "series.h"
#include "study.h"
#include "mathtools.h"
#include "volumepixeldataiterator.h"
#include "imageplane.h"
#include "dicomtagreader.h"
#include "volumehelper.h"

namespace udg {

Volume::Volume(QObject *parent)
: QObject(parent), m_checkedImagesAnatomicalPlane(false)
{
    m_numberOfPhases = 1;
    m_numberOfSlicesPerPhase = 1;

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
}

void Volume::setData(vtkImageData *vtkImage)
{
    m_volumePixelData->setData(vtkImage);
}

void Volume::setPixelData(VolumePixelData *pixelData)
{
    Q_ASSERT(pixelData != 0);
    m_volumePixelData = pixelData;
    // Set the number of phases to the new pixel data
    m_volumePixelData->setNumberOfPhases(m_numberOfPhases);
}

VolumePixelData* Volume::getPixelData()
{
    if (!isPixelDataLoaded())
    {
        VolumeReader *volumeReader = createVolumeReader();
        connect(volumeReader, SIGNAL(progress(int)), SIGNAL(progress(int)));
        volumeReader->read(this);
        delete volumeReader;

        // Set the number of phases to the new pixel data
        m_volumePixelData->setNumberOfPhases(m_numberOfPhases);
    }

    return m_volumePixelData;
}

bool Volume::isPixelDataLoaded() const
{
    return m_volumePixelData && m_volumePixelData->isLoaded();
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

void Volume::getExtent(int extent[6])
{
    getVtkData()->GetExtent(extent);
}

int* Volume::getExtent()
{
    return getVtkData()->GetExtent();
}

int* Volume::getDimensions()
{
    return getVtkData()->GetDimensions();
}

void Volume::getDimensions(int dims[3])
{
    getVtkData()->GetDimensions(dims);
}

std::array<Vector3, 8> Volume::getCorners()
{
    double *bounds = getVtkData()->GetBounds();
    double xMin = bounds[0], xMax = bounds[1], yMin = bounds[2], yMax = bounds[3], zMin = bounds[4], zMax = bounds[5];

    if (getNumberOfPhases() > 1)    // correction for multi-phase volumes
    {
        int extent5 = getExtent()[4] + getDimensions()[2] / getNumberOfPhases() - 1;
        zMax = getOrigin()[2] + extent5 * getSpacing()[2];
    }

    return {{Vector3(xMin, yMin, zMin), Vector3(xMax, yMin, zMin), Vector3(xMin, yMax, zMin), Vector3(xMax, yMax, zMin),
             Vector3(xMin, yMin, zMax), Vector3(xMax, yMin, zMax), Vector3(xMin, yMax, zMax), Vector3(xMax, yMax, zMax)}};
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

        // Set the number of phases to the pixel data only if it's already loaded, because we don't want to load it now
        if (isPixelDataLoaded())
        {
            getPixelData()->setNumberOfPhases(m_numberOfPhases);
        }
    }
}

int Volume::getNumberOfPhases() const
{
    return m_numberOfPhases;
}

Volume* Volume::getPhaseVolume(int index)
{
    Volume *result = 0;
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

QString Volume::getModality() const
{
    QString modality;

    if (getSeries())
    {
        modality = getSeries()->getModality();
    }
    
    return modality;
}

void Volume::addImage(Image *image)
{
    if (!m_imageSet.contains(image))
    {
        m_imageSet << image;
        // Si tenim dades carregades passen a ser invàlides
        if (isPixelDataLoaded())
        {
            // WARNING Possible memory leak temporal: el VolumePixelData anterior quedarà penjat sense destruir fins que es destruixi el seu pare (si en té,
            // sinó per sempre).
            m_volumePixelData = new VolumePixelData(this);
        }

        m_checkedImagesAnatomicalPlane = false;
    }
}

void Volume::setImages(const QList<Image*> &imageList)
{
    m_imageSet.clear();
    m_imageSet = imageList;
    // Si tenim dades carregades passen a ser invàlides
    if (isPixelDataLoaded())
    {
        // WARNING Possible memory leak temporal: el VolumePixelData anterior quedarà penjat sense destruir fins que es destruixi el seu pare (si en té, sinó
        // per sempre).
        m_volumePixelData = new VolumePixelData(this);
    }

    m_checkedImagesAnatomicalPlane = false;
}

QList<Image*> Volume::getImages() const
{
    return m_imageSet;
}

int Volume::getNumberOfFrames() const
{
    return m_imageSet.count();
}

bool Volume::isMultiframe() const
{
    if (m_imageSet.count() > 1)
    {
        // Comprovant la primera i segona imatges n'hi ha prou
        if (m_imageSet.at(0)->getPath() == m_imageSet.at(1)->getPath())
        {
            return true;
        }
    }

    return false;
}

Series* Volume::getSeries() const
{
    if (!m_imageSet.isEmpty())
    {
        return m_imageSet.at(0)->getParentSeries();
    }
    else
    {
        return 0;
    }
}

Study* Volume::getStudy() const
{
    Series *series = getSeries();
    if (series)
    {
        return series->getParentStudy();
    }
    else
    {
        return 0;
    }
}

Patient* Volume::getPatient() const
{
    if (this->getStudy())
    {
        return this->getStudy()->getParentPatient();
    }
    else
    {
        return 0;
    }
}

QString Volume::toString(bool verbose)
{
    Q_UNUSED(verbose);
    QString result;

    if (isPixelDataLoaded())
    {
        int dims[3];
        double origin[3];
        double spacing[3];
        int extent[6];
        double bounds[6];

        this->getDimensions(dims);
        this->getOrigin(origin);
        this->getSpacing(spacing);
        this->getExtent(extent);
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
    Image *image = 0;

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

QString Volume::getPixelUnits()
{
    QString units;
    Image *image = getImage(0);
    if (image)
    {
        if (image->getParentSeries())
        {
            QString modality = image->getParentSeries()->getModality();
            if (VolumeHelper::isPrimaryCT(this))
            {
                units = "HU";
            }
            else if (modality == "PT")
            {
                units = getPTPixelUnits(image);
            }
            else if (VolumeHelper::isPrimaryNM(this))
            {
                units = tr("counts");
            }
        }
    }

    return units;
}

QString Volume::getPTPixelUnits(const Image *image)
{
    if (m_PTPixelUnits.isNull())
    {
        QString dicomUnits = DICOMTagReader(image->getPath()).getValueAttributeAsQString(DICOMUnits);

        if (dicomUnits == "CNTS")
        {
            m_PTPixelUnits = tr("counts");
        }
        else if (dicomUnits == "NONE")
        {
            m_PTPixelUnits = "";
        }
        else if (dicomUnits == "CM2")
        {
            m_PTPixelUnits = "cm2";
        }
        else if (dicomUnits == "PCNT")
        {
            m_PTPixelUnits = tr("percent");
        }
        else if (dicomUnits == "CPS")
        {
            m_PTPixelUnits = tr("counts/s");
        }
        else if (dicomUnits == "BQML")
        {
            m_PTPixelUnits = "bq/ml";
        }
        else if (dicomUnits == "MGMINML")
        {
            m_PTPixelUnits = "mg/min/ml";
        }
        else if (dicomUnits == "UMOLMINML")
        {
            m_PTPixelUnits = "umol/min/ml";
        }
        else if (dicomUnits == "MLMING")
        {
            m_PTPixelUnits = "ml/min/g";
        }
        else if (dicomUnits == "MLG")
        {
            m_PTPixelUnits = "ml/g";
        }
        else if (dicomUnits == "1CM")
        {
            m_PTPixelUnits = "1/cm";
        }
        else if (dicomUnits == "UMOLML")
        {
            m_PTPixelUnits = "umol/ml";
        }
        else if (dicomUnits == "PROPCNTS")
        {
            m_PTPixelUnits = tr("proportional to counts");
        }
        else if (dicomUnits == "PROPCPS")
        {
            m_PTPixelUnits = "proportional to counts/s";
        }
        else if (dicomUnits == "MLMINML")
        {
            m_PTPixelUnits = "ml/min/ml";
        }
        else if (dicomUnits == "ML/ML")
        {
            m_PTPixelUnits = "ml/ml";
        }
        else if (dicomUnits == "GML")
        {
            m_PTPixelUnits = "g/ml";
        }
        else if (dicomUnits == "STDDEV")
        {
            m_PTPixelUnits = "std. dev";
        }
        else
        {
            m_PTPixelUnits = "";
        }
    }

    return m_PTPixelUnits;
}

QSharedPointer<ImagePlane> Volume::getImagePlane(int sliceNumber, const OrthogonalPlane &plane, bool vtkReconstructionHack)
{
    QSharedPointer<ImagePlane> imagePlane;
    int *dimensions = getDimensions();
    double *spacing = getSpacing();
    const double *origin = getOrigin();
    
    switch (plane)
    {
        case OrthogonalPlane::XYPlane:
        {
            Image *image = getImage(sliceNumber);
            if (image)
            {
                imagePlane.reset(new ImagePlane());
                imagePlane->fillFromImage(image);
            }
        }
            break;

        case OrthogonalPlane::YZPlane:
        {
            Image *image = getImage(0);
            if (image)
            {
                Vector3 sagittalRowVector = image->getImageOrientationPatient().getColumnVector();
                Vector3 sagittalColumnVector;
                if (vtkReconstructionHack)
                {
                    // Returning a fake plane, regarding real world coords, but fits better to vtk world
                    sagittalColumnVector = image->getImageOrientationPatient().getNormalVector();
                }
                else
                {
                    // This would be the norm, returning the real plane direction
                    getStackDirection(sagittalColumnVector.data(), 0);
                }

                imagePlane.reset(new ImagePlane());
                imagePlane->setImageOrientation(ImageOrientation(sagittalRowVector, sagittalColumnVector));
                imagePlane->setSpacing(PixelSpacing2D(spacing[1], spacing[2]));
                imagePlane->setThickness(spacing[0]);
                imagePlane->setRowLength(dimensions[1] * spacing[1]);
                imagePlane->setColumnLength(dimensions[2] * spacing[2]);

                Vector3 sagittalNormalVector = image->getImageOrientationPatient().getRowVector();
                imagePlane->setOrigin(Vector3(origin) + sliceNumber * sagittalNormalVector * spacing[0]);
            }
        }
            break;

        case OrthogonalPlane::XZPlane:
        {
            Image *image = getImage(0);
            if (image)
            {
                Vector3 coronalRowVector = image->getImageOrientationPatient().getRowVector();
                Vector3 coronalColumnVector;
                if (vtkReconstructionHack)
                {
                    // Returning a fake plane, regarding real world coords, but fits better to vtk world
                    coronalColumnVector = image->getImageOrientationPatient().getNormalVector();
                }
                else
                {
                    // This would be the norm, returning the real plane direction
                    getStackDirection(coronalColumnVector.data(), 0);
                }

                imagePlane.reset(new ImagePlane());
                imagePlane->setImageOrientation(ImageOrientation(coronalRowVector, coronalColumnVector));
                imagePlane->setSpacing(PixelSpacing2D(spacing[0], spacing[2]));
                imagePlane->setThickness(spacing[1]);
                imagePlane->setRowLength(dimensions[0] * spacing[0]);
                imagePlane->setColumnLength(dimensions[2] * spacing[2]);

                Vector3 coronalNormalVector = image->getImageOrientationPatient().getColumnVector();
                imagePlane->setOrigin(Vector3(origin) + coronalNormalVector * sliceNumber * spacing[1]);
            }
        }
            break;
    }
    
    return imagePlane;
}

void Volume::getSliceRange(int &min, int &max, const OrthogonalPlane &plane)
{
    if (m_numberOfPhases > 1 && plane == OrthogonalPlane::XYPlane)
    {
        min = 0;
        max = getNumberOfSlicesPerPhase() - 1;
    }
    else
    {
        int *extent = getExtent();
        min = extent[plane.getZIndex() * 2];
        max = extent[plane.getZIndex() * 2 + 1];
    }
}

int Volume::getMaximumSlice(const OrthogonalPlane &plane)
{
    int max, trash;
    this->getSliceRange(trash, max, plane);
    return max;
}

int Volume::getMinimumSlice(const OrthogonalPlane &plane)
{
    int min, trash;
    this->getSliceRange(min, trash, plane);
    return min;
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
        Vector3 normalVector = firstImage->getImageOrientationPatient().getNormalVector();
        direction[0] = normalVector.x;
        direction[1] = normalVector.y;
        direction[2] = normalVector.z;
    }
    else
    {
        const double *firstOrigin = firstImage->getImagePositionPatient();
        const double *secondOrigin = secondImage->getImagePositionPatient();
        // Calculem la direcció real de com estan apilades
        Vector3 zDirection = MathTools::directorVector(firstOrigin, secondOrigin);
        zDirection.normalize();
        direction[0] = zDirection.x;
        direction[1] = zDirection.y;
        direction[2] = zDirection.z;
    }
}

void* Volume::getScalarPointer(int x, int y, int z)
{
    return this->getPixelData()->getScalarPointer(x, y, z);
}

void* Volume::getScalarPointer(int index[3])
{
    return this->getScalarPointer(index[0], index[1], index[2]);
}

VolumePixelDataIterator Volume::getIterator(int x, int y, int z)
{
    return this->getPixelData()->getIterator(x, y, z);
}

VolumePixelDataIterator Volume::getIterator()
{
    return this->getPixelData()->getIterator();
}

double Volume::getScalarValue(int x, int y, int z)
{
    return *static_cast<double*>(this->getPixelData()->getScalarPointer(x, y, z));
}

void Volume::convertToNeutralVolume()
{
    m_volumePixelData->convertToNeutralPixelData();

    // Quan creem el volum neutre indiquem que només tenim 1 sola fase
    // TODO Potser s'haurien de crear tantes fases com les que indiqui la sèrie?
    this->setNumberOfPhases(1);
}

AnatomicalPlane Volume::getAcquisitionPlane() const
{
    if (m_imageSet.isEmpty())
    {
        return AnatomicalPlane::NotAvailable;
    }
    else
    {
        return AnatomicalPlane::getPlaneFromPatientOrientation(m_imageSet.first()->getPatientOrientation());
    }
}

OrthogonalPlane Volume::getCorrespondingOrthogonalPlane(const AnatomicalPlane &anatomicalPlane) const
{
    OrthogonalPlane orthogonalPlane;
    AnatomicalPlane acquisitionPlane = getAcquisitionPlane();
    switch (acquisitionPlane)
    {
        case AnatomicalPlane::Axial:
        case AnatomicalPlane::NotAvailable:
        case AnatomicalPlane::Oblique:
            switch(anatomicalPlane)
            {
                case AnatomicalPlane::Axial:
                    orthogonalPlane = OrthogonalPlane::XYPlane;
                    break;

                case AnatomicalPlane::Sagittal:
                    orthogonalPlane = OrthogonalPlane::YZPlane;
                    break;

                case AnatomicalPlane::Coronal:
                    orthogonalPlane = OrthogonalPlane::XZPlane;
                    break;

                default:
                    orthogonalPlane = OrthogonalPlane::XYPlane;
                    break;
            }
            break;

        case AnatomicalPlane::Sagittal:
            switch(anatomicalPlane)
            {
                case AnatomicalPlane::Axial:
                    orthogonalPlane = OrthogonalPlane::XZPlane;
                    break;

                case AnatomicalPlane::Sagittal:
                    orthogonalPlane = OrthogonalPlane::XYPlane;
                    break;

                case AnatomicalPlane::Coronal:
                    orthogonalPlane = OrthogonalPlane::YZPlane;
                    break;

                default:
                    orthogonalPlane = OrthogonalPlane::XYPlane;
                    break;
            }
            break;

        case AnatomicalPlane::Coronal:
            switch(anatomicalPlane)
            {
                case AnatomicalPlane::Axial:
                    orthogonalPlane = OrthogonalPlane::XZPlane;
                    break;

                case AnatomicalPlane::Sagittal:
                    orthogonalPlane = OrthogonalPlane::YZPlane;
                    break;

                case AnatomicalPlane::Coronal:
                    orthogonalPlane = OrthogonalPlane::XYPlane;
                    break;

                default:
                    orthogonalPlane = OrthogonalPlane::XYPlane;
                    break;
            }
            break;
    }

    return orthogonalPlane;
}

int Volume::getImageIndex(int sliceNumber, int phaseNumber) const
{
    return sliceNumber * m_numberOfPhases + phaseNumber;
}

VolumeReader* Volume::createVolumeReader()
{
    return new VolumeReader(this);
}

bool Volume::computeCoordinateIndex(const double coordinate[3], int index[3])
{
    return getPixelData()->computeCoordinateIndex(coordinate, index);
}

int Volume::getNumberOfScalarComponents()
{
    return this->getPixelData()->getNumberOfScalarComponents();
}

int Volume::getScalarSize()
{
    return this->getPixelData()->getScalarSize();
}

QByteArray Volume::getImageScalarPointer(int imageNumber)
{
    int *dimensions = getDimensions();
    int bytesPerImage = getScalarSize() * getNumberOfScalarComponents() * dimensions[0] * dimensions[1];

    const char *scalarPointer = reinterpret_cast<const char*>(this->getScalarPointer());

    scalarPointer += bytesPerImage*imageNumber;

    return QByteArray(scalarPointer,bytesPerImage);
}

bool Volume::areAllImagesInTheSameAnatomicalPlane() const
{
    if (!m_checkedImagesAnatomicalPlane)
    {
        m_checkedImagesAnatomicalPlane = true;
        m_allImagesAreInTheSameAnatomicalPlane = true;

        if (!m_imageSet.isEmpty())
        {
            AnatomicalPlane anatomicalPlane = AnatomicalPlane::getPlaneFromPatientOrientation(m_imageSet.first()->getPatientOrientation());

            foreach (Image *image, m_imageSet)
            {
                if (AnatomicalPlane::getPlaneFromPatientOrientation(image->getPatientOrientation()) != anatomicalPlane)
                {
                    m_allImagesAreInTheSameAnatomicalPlane = false;
                    break;
                }
            }
        }
    }

    return m_allImagesAreInTheSameAnatomicalPlane;
}

};
