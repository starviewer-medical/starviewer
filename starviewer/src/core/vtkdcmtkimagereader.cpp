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

#include "vtkdcmtkimagereader.h"

#include "dicomsequenceattribute.h"
#include "dicomsequenceitem.h"
#include "dicomtagreader.h"
#include "dicomvalueattribute.h"
#include "logging.h"
#include "mathtools.h"
#include "photometricinterpretation.h"
#include "imageorientation.h"

#include <QSharedPointer>
#include <QStringList>

#include <vtkDataArray.h>
#include <vtkImageCast.h>
#include <vtkImageData.h>
#include <vtkInformation.h>
#include <vtkInformationVector.h>
#include <vtkObjectFactory.h>
#include <vtkPointData.h>
#include <vtkStreamingDemandDrivenPipeline.h>
#include <vtkStringArray.h>

#include <dcfilefo.h>   // DcmFileFormat
#include <dcmimage.h>   // DicomImage

namespace udg {

namespace {

// This exception is thrown when a file can't be loaded.
class CantLoadFileException {
};

// This exception is thrown when the current data scalar type has to be changed.
class ChangeScalarTypeException {
public:
    ChangeScalarTypeException(int newScalarType) :
        m_newScalarType(newScalarType)
    {
    }
    // Returns the new scalar type.
    int getNewScalarType() const
    {
        return m_newScalarType;
    }
private:
    // New scalar type represented as a VTK constant.
    int m_newScalarType;
};

// Reads the tag from the given functional groups sequence. Returns null string if it hasn't read.
QString getTagValueFromFunctionalGroupsSequence(DICOMSequenceAttribute *functionalGroupsSequence, int index, const DICOMTag &sequenceTag, const DICOMTag &tag)
{
    QString value;

    if (functionalGroupsSequence)
    {
        if (index < functionalGroupsSequence->getItems().size())
        {
            DICOMSequenceItem *item = functionalGroupsSequence->getItems().at(index);
            DICOMSequenceAttribute *sequence = item->getSequenceAttribute(sequenceTag);

            if (sequence)
            {
                if (!sequence->getItems().isEmpty())
                {
                    DICOMSequenceItem *sequenceItem = sequence->getItems().first();
                    DICOMValueAttribute *valueAttribute = sequenceItem->getValueAttribute(tag);

                    if (valueAttribute)
                    {
                        value = valueAttribute->getValueAsQString();
                    }
                }
            }
        }
    }

    return value;
};

// Reads and returns the value of a tag from the given DICOM tag reader. It first tries to read the tag from the per-frame functional groups sequence (from the
// item at the given index); if not found, it tries to read the tag from the shared functional groups sequence; if not found, it tries to read the tag from the
// root; if not found, it returns a null QString. The sequence tag selects the sequence inside the functional groups sequences where the tag has to be searched.
QString getTagValue(const DICOMTagReader &dicomTagReader, int index, const DICOMTag &sequenceTag, const DICOMTag &tag)
{
    // Try to read from the per-frame functional groups sequence
    DICOMSequenceAttribute *sequence = dicomTagReader.getSequenceAttribute(DICOMPerFrameFunctionalGroupsSequence);
    QString value = getTagValueFromFunctionalGroupsSequence(sequence, index, sequenceTag, tag);
    if (!value.isNull())
    {
        return value;
    }

    // Try to read from the shared functional groups sequence
    sequence = dicomTagReader.getSequenceAttribute(DICOMSharedFunctionalGroupsSequence);
    index = 0;
    value = getTagValueFromFunctionalGroupsSequence(sequence, index, sequenceTag, tag);
    if (!value.isNull())
    {
        return value;
    }

    // Try to read from the root
    return dicomTagReader.getValueAttributeAsQString(tag);
}

// Returns the suitable VTK scalar type for the given bits stored and pixel representation.
int getSuitableScalarType(int bitsStored, int pixelRepresentation)
{
    int bytesStored = MathTools::roundUpToPowerOf2(bitsStored) / 8;

    switch (bytesStored)
    {
        case 1: return pixelRepresentation == 0 ? VTK_UNSIGNED_CHAR : VTK_SIGNED_CHAR;
        case 2: return pixelRepresentation == 0 ? VTK_UNSIGNED_SHORT : VTK_SHORT;
        case 4: return pixelRepresentation == 0 ? VTK_UNSIGNED_INT : VTK_INT;
        default: throw std::invalid_argument(qPrintable(QString("Unexpected bytes stored: %1").arg(bytesStored)));
    }
}

// Converts the given DCMTK representation to the equivalent VTK scalar type constant.
int dcmtkRepresentationToVtkScalarType(EP_Representation representation)
{
    switch (representation)
    {
        case EPR_Uint8: return VTK_UNSIGNED_CHAR;
        case EPR_Sint8: return VTK_SIGNED_CHAR;
        case EPR_Uint16: return VTK_UNSIGNED_SHORT;
        case EPR_Sint16: return VTK_SHORT;
        case EPR_Uint32: return VTK_UNSIGNED_INT;
        case EPR_Sint32: return VTK_INT;
        case EPR_float32: return VTK_FLOAT;
        default: throw std::invalid_argument("Unexpected representation");  // Should not happen
    }
}

// Returns true if the original VTK scalar type, having the current maximum value, can be converted to the target one without overflow.
bool canConvertScalarType(int original, int target, double maximum)
{
    return (original == VTK_UNSIGNED_CHAR && (target == VTK_UNSIGNED_CHAR ||
                                             (target == VTK_SIGNED_CHAR && maximum <= SCHAR_MAX) ||
                                              target == VTK_UNSIGNED_SHORT ||
                                              target == VTK_SHORT ||
                                              target == VTK_UNSIGNED_INT ||
                                              target == VTK_INT ||
                                              target == VTK_FLOAT))
        || (original == VTK_SIGNED_CHAR && (target == VTK_SIGNED_CHAR ||
                                            target == VTK_SHORT ||
                                            target == VTK_INT ||
                                            target == VTK_FLOAT))
        || (original == VTK_UNSIGNED_SHORT && (target == VTK_UNSIGNED_SHORT ||
                                              (target == VTK_SHORT && maximum <= SHRT_MAX) ||
                                               target == VTK_UNSIGNED_INT ||
                                               target == VTK_INT ||
                                               target == VTK_FLOAT))
        || (original == VTK_SHORT && (target == VTK_SHORT ||
                                      target == VTK_INT ||
                                      target == VTK_FLOAT))
        || (original == VTK_UNSIGNED_INT && (target == VTK_UNSIGNED_INT ||
                                            (target == VTK_INT && maximum <= INT_MAX) ||
                                             target == VTK_FLOAT))
        || (original == VTK_INT && (target == VTK_INT ||
                                    target == VTK_FLOAT))
        || (original == VTK_FLOAT && target == VTK_FLOAT);
}

// Given two scalar types and a maximum value, returns the smaller new scalar type that can hold all the existent values.
int decideNewScalarType(int scalarType1, int scalarType2, double maximum)
{
    // Try unsigned types first, and from smaller to bigger
    int scalarTypesToTry[7] = { VTK_UNSIGNED_CHAR, VTK_UNSIGNED_SHORT, VTK_UNSIGNED_INT, VTK_SIGNED_CHAR, VTK_SHORT, VTK_INT, VTK_FLOAT };

    for (int i = 0; i < 7; i++)
    {
        if (canConvertScalarType(scalarType1, scalarTypesToTry[i], maximum) && canConvertScalarType(scalarType2, scalarTypesToTry[i], maximum))
        {
            return scalarTypesToTry[i];
        }
    }

    // If we reach this point it means the we can't store all the values as int or unsigned int; this should be very rare
    // For now, let's throw an exception
    throw std::overflow_error("Can't find a suitable scalar type for the whole data");
}

// Returns the voxel size in bytes for the given scalar type and number of components.
size_t voxelSize(int scalarType, int numberOfComponents)
{
    size_t componentSize = 1;

    switch (scalarType)
    {
        case VTK_UNSIGNED_CHAR:
        case VTK_SIGNED_CHAR:
            componentSize = sizeof(char);
            break;
        case VTK_UNSIGNED_SHORT:
        case VTK_SHORT:
            componentSize = sizeof(short);
            break;
        case VTK_UNSIGNED_INT:
        case VTK_INT:
            componentSize = sizeof(int);
            break;
        case VTK_FLOAT:
            componentSize = sizeof(float);
            break;
    }

    return componentSize * numberOfComponents;
}

// Returns a DcmDataset initialized from the given filename.
QSharedPointer<DcmDataset> getDataset(const char *filename)
{
    DcmFileFormat dicomFile;
    OFCondition status = dicomFile.loadFile(filename);

    if (status.bad())
    {
        ERROR_LOG(QString("Can't load file %1: %2").arg(filename).arg(status.text()));
        throw CantLoadFileException();
    }

    return QSharedPointer<DcmDataset>(dicomFile.getAndRemoveDataset());
}

const char* booleanToString(bool b)
{
    return b ? "yes" : "no";
};

} // namespace

vtkStandardNewMacro(VtkDcmtkImageReader);

void VtkDcmtkImageReader::PrintSelf(std::ostream &os, vtkIndent indent)
{
    this->Superclass::PrintSelf(os, indent);

    os << indent << "Is multiframe: " << booleanToString(m_isMultiframe) << "\n";
    os << indent << "Has per-frame rescale: " << booleanToString(m_hasPerFrameRescale) << "\n";
    os << indent << "Is monochrome: " << booleanToString(m_isMonochrome) << "\n";
    os << indent << "Frame size: " << m_frameSize << " bytes\n";
    os << indent << "Maximum voxel value: " << m_maximumVoxelValue << "\n";
    os << indent << "Needs float scalar type: " << booleanToString(m_needsFloatScalarType) << "\n";
}

void VtkDcmtkImageReader::setFrameNumbers(const QList<int> &frameNumbers)
{
    m_frameNumbers = frameNumbers;
}

VtkDcmtkImageReader::VtkDcmtkImageReader()
{
    this->SetNumberOfInputPorts(0);
    this->SetNumberOfOutputPorts(1);
}

int VtkDcmtkImageReader::RequestInformation(vtkInformation *vtkNotUsed(request), vtkInformationVector **vtkNotUsed(inputVector),
                                            vtkInformationVector *outputVector)
{
    // Let's read the first file
    QString filename;

    if (this->FileName)
    {
        filename = this->FileName;
    }
    else if (this->FileNames && this->FileNames->GetNumberOfValues() > 0)
    {
        filename = this->FileNames->GetValue(0);
    }
    else
    {
        // Hey! I need at least one file to schew on!
        ERROR_LOG("No filename given");
        return 0;
    }

    // At the beginning we don't need a float scalar type. This will be set to true by the upcoming methods if needed.
    m_needsFloatScalarType = false;

    if (!readInformation(filename))
    {
        ERROR_LOG("Error reading information.");
        return 0;
    }

    if (!decideInitialScalarTypeAndNumberOfComponents())
    {
        throw CantReadImageException("Can't decide a scalar type for the image. This may be due to corrupt data.");
    }

    this->SetNumberOfOutputPorts(1);

    // Allocate!
    if (!this->GetOutput(0))
    {
        vtkImageData *imageData = vtkImageData::New();
        this->GetExecutive()->SetOutputData(0, imageData);
        imageData->Delete();
    }

    vtkInformation *outputInformation = outputVector->GetInformationObject(0);
    outputInformation->Set(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(), this->DataExtent, 6);
    outputInformation->Set(vtkDataObject::SPACING(), this->DataSpacing, 3);
    outputInformation->Set(vtkDataObject::ORIGIN(), this->DataOrigin, 3);
    vtkDataObject::SetPointDataActiveScalarInfo(outputInformation, this->DataScalarType, this->NumberOfScalarComponents);

    return 1;
}

int VtkDcmtkImageReader::RequestData(vtkInformation *vtkNotUsed(request), vtkInformationVector **vtkNotUsed(inputVector), vtkInformationVector *outputVector)
{
    vtkInformation *outputInformation = outputVector->GetInformationObject(0);

    // Make sure the output dimension is OK, and allocate its scalars
    int updateExtent[6];
    outputInformation->Get(vtkStreamingDemandDrivenPipeline::UPDATE_EXTENT(), updateExtent);

    bool retry;

    // The do-while construct allows to restart the reading with a new pixel type
    do
    {
        int dimX = updateExtent[1] - updateExtent[0] + 1;
        int dimY = updateExtent[3] - updateExtent[2] + 1;
        m_frameSize = dimX * dimY * voxelSize(this->DataScalarType, this->NumberOfScalarComponents);
        m_maximumVoxelValue = 0.0;
        retry = false;

        try
        {
            if (!loadData(updateExtent))
            {
                return 0;
            }
        }
        catch (const CantLoadFileException &)
        {
            return 0;
        }
        catch (const ChangeScalarTypeException &exception)
        {
            this->DataScalarType = exception.getNewScalarType();
            vtkDataObject::SetPointDataActiveScalarInfo(outputInformation, this->DataScalarType, this->NumberOfScalarComponents);
            retry = true;
        }
    }
    while (retry);

    return 1;
}

bool VtkDcmtkImageReader::readInformation(const QString &filename)
{
    DICOMTagReader dicomTagReader(filename);

    if (!dicomTagReader.canReadFile())
    {
        return false;
    }

    readExtent(dicomTagReader);
    readSpacing(dicomTagReader);
    readOrigin(dicomTagReader);

    // If we have a multiframe volume, read all per-frame rescale values now and keep them for later
    // Reading them individually while reading data is too slow
    if (m_isMultiframe)
    {
        readPerFrameRescale(dicomTagReader);
    }

    return true;
}

void VtkDcmtkImageReader::readExtent(const DICOMTagReader &dicomTagReader)
{
    int columns = dicomTagReader.getValueAttributeAsQString(DICOMColumns).toInt();
    int rows = dicomTagReader.getValueAttributeAsQString(DICOMRows).toInt();
    int frames = 1;

    m_isMultiframe = false;

    if (this->FileName && dicomTagReader.tagExists(DICOMNumberOfFrames))
    {
        frames = dicomTagReader.getValueAttributeAsQString(DICOMNumberOfFrames).toInt();
        m_isMultiframe = true;
    }
    else if (this->FileNames && this->FileNames->GetNumberOfValues() > 1)
    {
        frames = this->FileNames->GetNumberOfValues();
    }

    this->DataExtent[0] = 0;
    this->DataExtent[1] = columns - 1;
    this->DataExtent[2] = 0;
    this->DataExtent[3] = rows - 1;
    this->DataExtent[4] = 0;
    this->DataExtent[5] = frames - 1;

    if (frames > 1)
    {
        // File dimensionality is 2 by default
        this->FileDimensionality = 3;
    }
}

void VtkDcmtkImageReader::readSpacing(const DICOMTagReader &dicomTagReader)
{
    this->DataSpacing[0] = this->DataSpacing[1] = this->DataSpacing[2] = 1.0;
    readPixelSpacing(dicomTagReader);
    readSpacingBetweenSlices(dicomTagReader);
}

void VtkDcmtkImageReader::readPixelSpacing(const DICOMTagReader &dicomTagReader)
{
    QString pixelSpacing = getTagValue(dicomTagReader, 0, DICOMPixelMeasuresSequence, DICOMPixelSpacing);

    if (pixelSpacing.isNull())
    {
        if (dicomTagReader.tagExists(DICOMImagerPixelSpacing))
        {
            pixelSpacing = dicomTagReader.getValueAttributeAsQString(DICOMImagerPixelSpacing);
        }
        else if (dicomTagReader.tagExists(DICOMNominalScannedPixelSpacing))
        {
            pixelSpacing = dicomTagReader.getValueAttributeAsQString(DICOMNominalScannedPixelSpacing);
        }
    }

    if (!pixelSpacing.isNull())
    {
        QStringList pixelSpacingStrings = pixelSpacing.split("\\");

        if (pixelSpacingStrings.size() == 2)
        {
            double rowSpacing = pixelSpacingStrings.at(0).toDouble();
            double columnSpacing = pixelSpacingStrings.at(1).toDouble();

            // Pixel spacing is rowSpacing\columnSpacing -> ySpacing\xSpacing
            if (columnSpacing != 0.0)
            {
                this->DataSpacing[0] = columnSpacing;
            }
            if (rowSpacing != 0.0)
            {
                this->DataSpacing[1] = rowSpacing;
            }
        }
        else
        {
            DEBUG_LOG("Unexpected pixel spacing: " + pixelSpacing);
            WARN_LOG("Unexpected pixel spacing: " + pixelSpacing);
        }
    }
}

void VtkDcmtkImageReader::readSpacingBetweenSlices(const DICOMTagReader &dicomTagReader)
{
    if (dicomTagReader.tagExists(DICOMSpacingBetweenSlices))
    {
        this->DataSpacing[2] = dicomTagReader.getValueAttributeAsQString(DICOMSpacingBetweenSlices).toDouble();
    }
}

void VtkDcmtkImageReader::readOrigin(const DICOMTagReader &dicomTagReader)
{
    this->DataOrigin[0] = this->DataOrigin[1] = this->DataOrigin[2] = 0.0;

    int firstFrame = m_frameNumbers.isEmpty()? 0 : m_frameNumbers.first();
    QString imagePositionPatient = getTagValue(dicomTagReader, firstFrame, DICOMPlanePositionSequence, DICOMImagePositionPatient);

    if (imagePositionPatient.isEmpty() && dicomTagReader.getValueAttributeAsQString(DICOMModality) == "NM")
    {
        QString imageType = dicomTagReader.getValueAttributeAsQString(DICOMImageType);

        if (imageType.contains("PRIMARY\\RECON") && dicomTagReader.tagExists(DICOMDetectorInformationSequence))
        {
            DICOMSequenceAttribute *detectorsSequence = dicomTagReader.getSequenceAttribute(DICOMDetectorInformationSequence);

            if (detectorsSequence->getItems().size() == 1)
            {
                DICOMSequenceItem *firstDetectorItem = detectorsSequence->getItems().first();
                QString imageOrientationString = firstDetectorItem->getValueAttribute(DICOMImageOrientationPatient)->getValueAsQString();
                QString imagePositionPatientString = firstDetectorItem->getValueAttribute(DICOMImagePositionPatient)->getValueAsQString();
                double spacingBetweenSlices = dicomTagReader.getValueAttributeAsQString(DICOMSpacingBetweenSlices).toDouble();

                if (!imageOrientationString.isEmpty() && !imagePositionPatientString.isEmpty() && spacingBetweenSlices != 0.0)
                {
                    ImageOrientation imageOrientation;
                    imageOrientation.setDICOMFormattedImageOrientation(imageOrientationString);

                    QStringList values = imagePositionPatientString.split("\\");
                    Vector3 firstFrameImagePositionPatient(values.at(0).toDouble(), values.at(1).toDouble(), values.at(2).toDouble());
                    Vector3 imagePosition = firstFrameImagePositionPatient + m_frameNumbers.first() * spacingBetweenSlices * imageOrientation.getNormalVector();

                    imagePositionPatient = QString("%1\\%2\\%3").arg(imagePosition.x).arg(imagePosition.y).arg(imagePosition.z);
                    ERROR_LOG("Unexpected image position patient: " + imagePositionPatient);
                }
            }
        }
    }

    if (!imagePositionPatient.isNull())
    {
        QStringList imagePositionPatientStrings = imagePositionPatient.split("\\");

        if (imagePositionPatientStrings.size() == 3)
        {
            for (int i = 0; i < 3; i++)
            {
                this->DataOrigin[i] = imagePositionPatientStrings.at(i).toDouble();
            }
        }
        else
        {
            DEBUG_LOG("Unexpected image position patient: " + imagePositionPatient);
            WARN_LOG("Unexpected image position patient: " + imagePositionPatient);
        }
    }
}

void VtkDcmtkImageReader::readPerFrameRescale(const DICOMTagReader &dicomTagReader)
{
    m_hasPerFrameRescale = false;
    DICOMSequenceAttribute *sequence = dicomTagReader.getSequenceAttribute(DICOMPerFrameFunctionalGroupsSequence);

    if (sequence)
    {
        m_perFrameRescale.clear();
        DICOMSequenceAttribute *pixelValueTransformationSequencInSharedSequence = NULL;

        // Checking if the functional group is located in Shared Functional Groups Sequence.
        // In that case, we won't look into Per-Frame Functional Groups.
        DICOMSequenceAttribute *sharedFunctionalGroupsSequence = dicomTagReader.getSequenceAttribute(DICOMSharedFunctionalGroupsSequence);
        if (sharedFunctionalGroupsSequence)
        {
            QList<DICOMSequenceItem*> sharedItems = dicomTagReader.getSequenceAttribute(DICOMSharedFunctionalGroupsSequence)->getItems();
            if (!sharedItems.isEmpty())
            {
                pixelValueTransformationSequencInSharedSequence = sharedItems.first()->getSequenceAttribute(DICOMPixelValueTransformationSequence);
            }
        }

        foreach (DICOMSequenceItem *item, sequence->getItems())
        {
            DICOMSequenceAttribute *subSequence;

            if (pixelValueTransformationSequencInSharedSequence)
            {
                subSequence = pixelValueTransformationSequencInSharedSequence;
            }
            else
            {
                subSequence = item->getSequenceAttribute(DICOMPixelValueTransformationSequence);
            }

            if (subSequence)
            {
                if (!subSequence->getItems().isEmpty())
                {
                    DICOMSequenceItem *subSequenceItem = subSequence->getItems().first();
                    DICOMValueAttribute *interceptValue = subSequenceItem->getValueAttribute(DICOMRescaleIntercept);
                    DICOMValueAttribute *slopeValue = subSequenceItem->getValueAttribute(DICOMRescaleSlope);

                    if (interceptValue && slopeValue)
                    {
                        Rescale rescale = { interceptValue->getValueAsDouble(), slopeValue->getValueAsDouble() };
                        m_perFrameRescale.append(rescale);
                        m_hasPerFrameRescale = true;

                        if (qAbs(rescale.slope) < 1.0)
                        {
                            m_needsFloatScalarType = true;
                        }
                    }
                }
            }
        }
    }
}

bool VtkDcmtkImageReader::decideInitialScalarTypeAndNumberOfComponents()
{
    bool hasDecidedScalarType = false;

    if (this->FileName)
    {
        hasDecidedScalarType = decideInitialScalarTypeAndNumberOfComponents(this->FileName);
    }
    else if (this->FileNames && this->FileNames->GetNumberOfValues() > 0)
    {
        for (int i = 0; !hasDecidedScalarType && i < this->FileNames->GetNumberOfValues(); i++)
        {
            hasDecidedScalarType = decideInitialScalarTypeAndNumberOfComponents(this->FileNames->GetValue(i));
        }
    }

    return hasDecidedScalarType;
}

bool VtkDcmtkImageReader::decideInitialScalarTypeAndNumberOfComponents(const char *filename)
{
    DICOMTagReader dicomTagReader(filename);

    if (!dicomTagReader.canReadFile())
    {
        return false;
    }

    PhotometricInterpretation photometricInterpretation(dicomTagReader.getValueAttributeAsQString(DICOMPhotometricInterpretation));

    if (!photometricInterpretation.isColor())
    {
        if (m_needsFloatScalarType)
        {
            this->DataScalarType = VTK_FLOAT;
        }
        else
        {
            int bitsStored = dicomTagReader.getValueAttributeAsQString(DICOMBitsStored).toInt();
            int pixelRepresentation = dicomTagReader.getValueAttributeAsQString(DICOMPixelRepresentation).toInt();
            this->DataScalarType = getSuitableScalarType(bitsStored, pixelRepresentation);
        }

        this->NumberOfScalarComponents = 1;
        m_isMonochrome = true;
    }
    else
    {
        this->DataScalarType = VTK_UNSIGNED_CHAR;
        this->NumberOfScalarComponents = 3;
        m_isMonochrome = false;
    }

    return true;
}

bool VtkDcmtkImageReader::loadData(int updateExtent[6])
{
    vtkImageData *output = this->GetOutput(0);
    output->SetExtent(updateExtent);
    output->AllocateScalars(this->GetOutputInformation(0));
    output->GetPointData()->GetScalars()->SetName("DCMTKImage");

    void *scalarPointer = output->GetScalarPointerForExtent(updateExtent);

    if (this->FileName)
    {
        if (!m_isMultiframe)
        {
            this->loadSingleFrameFile(this->FileName, scalarPointer);
        }
        else
        {
            this->loadMultiframeFile(this->FileName, scalarPointer, updateExtent);
        }
    }
    else if (this->FileNames && this->FileNames->GetNumberOfValues() > 0)
    {
        double total = updateExtent[5] - updateExtent[4] + 1;
        this->UpdateProgress(0.0);

        for (int i = updateExtent[4]; i <= updateExtent[5] && !this->AbortExecute; i++)
        {
            this->loadSingleFrameFile(this->FileNames->GetValue(i), scalarPointer);
            scalarPointer = static_cast<char*>(scalarPointer) + m_frameSize;
            this->UpdateProgress((i - updateExtent[4] + 1) / total);
        }
    }
    else
    {
        ERROR_LOG("No filename given");
        return false;
    }

    return !this->AbortExecute;
}

void VtkDcmtkImageReader::loadSingleFrameFile(const char *filename, void *buffer)
{
    QSharedPointer<DcmDataset> dataset = getDataset(filename);
    unsigned long flags = m_needsFloatScalarType ? CIF_UseFloatingInternalRepresentation : 0;
    DicomImage image(dataset.data(), dataset->getOriginalXfer(), flags);
    copyDcmtkImageToBuffer(buffer, image);
}

void VtkDcmtkImageReader::loadMultiframeFile(const char *filename, void *buffer, int updateExtent[6])
{
    QSharedPointer<DcmDataset> dataset = getDataset(filename);
    unsigned long flags = CIF_UsePartialAccessToPixelData | (m_needsFloatScalarType ? CIF_UseFloatingInternalRepresentation : 0);
    double total = updateExtent[5] - updateExtent[4] + 1;
    this->UpdateProgress(0.0);

    if (m_frameNumbers.isEmpty())
    {
        DEBUG_LOG("Reading multiframe file without frame numbers specified. Frames will be read sequentially.");
        WARN_LOG("Reading multiframe file without frame numbers specified. Frames will be read sequentially.");
    }

    for (int frameIndex = updateExtent[4]; frameIndex <= updateExtent[5] && !this->AbortExecute; frameIndex++)
    {
        int frameNumberInFile = m_frameNumbers.isEmpty() ? frameIndex : m_frameNumbers[frameIndex];

        if (m_hasPerFrameRescale)
        {
            const Rescale &rescale = m_perFrameRescale.at(frameNumberInFile);
            DicomImage image(dataset.data(), dataset->getOriginalXfer(), rescale.slope, rescale.intercept, flags, frameNumberInFile, 1);
            copyDcmtkImageToBuffer(buffer, image);
        }
        else
        {
            DicomImage image(dataset.data(), dataset->getOriginalXfer(), flags, frameNumberInFile, 1);
            copyDcmtkImageToBuffer(buffer, image);
        }

        buffer = static_cast<char*>(buffer) + m_frameSize;
        this->UpdateProgress((frameIndex - updateExtent[4] + 1) / total);
    }
}

void VtkDcmtkImageReader::copyDcmtkImageToBuffer(void *buffer, DicomImage &dicomImage)
{
    if (dicomImage.getStatus() != EIS_Normal)
    {
        ERROR_LOG("Found corrupt image or frame! Skipping copy.");
        return;
    }

    if (m_isMonochrome)
    {
        // Monochrome image, copy internal data
        const DiPixel * const dcmtkInternalData = dicomImage.getInterData();
        double minimum, maximum;
        dicomImage.getMinMaxValues(minimum, maximum);

        if (maximum > m_maximumVoxelValue)
        {
            m_maximumVoxelValue = maximum;
        }

        int dcmtkInternalDataScalarType = dcmtkRepresentationToVtkScalarType(dcmtkInternalData->getRepresentation());

        if (dcmtkInternalDataScalarType == this->DataScalarType)
        {
            // Internal data scalar type is the same as the image data scalar type, only need to copy
            memcpy(buffer, dcmtkInternalData->getData(), m_frameSize);
        }
        else if (canConvertScalarType(dcmtkInternalDataScalarType, this->DataScalarType, maximum))
        {
            // Internal data scalar type is different from the image data scalar type but can be converted to it
            // Need to create temporal image data, cast it to the current image data scalar type and copy
            vtkImageData *temporalImageData = vtkImageData::New();
            int frameExtent[6];
            this->GetDataExtent(frameExtent);
            frameExtent[4] = frameExtent[5] = 0;
            temporalImageData->SetExtent(frameExtent);
            temporalImageData->AllocateScalars(dcmtkInternalDataScalarType, 1);
            void *temporalImageDataBuffer = temporalImageData->GetScalarPointer();

            size_t readSize = dcmtkInternalData->getCount() * voxelSize(dcmtkInternalDataScalarType, 1);
            memcpy(temporalImageDataBuffer, dcmtkInternalData->getData(), readSize);

            vtkImageCast *imageCast = vtkImageCast::New();
            imageCast->SetInputData(temporalImageData);
            imageCast->SetOutputScalarType(this->DataScalarType);
            imageCast->Update();
            memcpy(buffer, imageCast->GetOutput()->GetScalarPointer(), m_frameSize);

            imageCast->Delete();
            temporalImageData->Delete();
        }
        else
        {
            // Internal data scalar type is different from the image data scalar type and can't be converted to it
            // Need to find a new scalar type suitable for both and restart read
            int newScalarType = decideNewScalarType(this->DataScalarType, dcmtkInternalDataScalarType, m_maximumVoxelValue);
            throw ChangeScalarTypeException(newScalarType);
        }
    }
    else
    {
        // Color image, use 8-bit RGB
        dicomImage.getOutputData(buffer, m_frameSize, 8);
    }
}

VtkDcmtkImageReader::CantReadImageException::CantReadImageException(const std::string &what) :
    std::runtime_error(what)
{
}

} // namespace udg
