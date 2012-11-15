#include "dicomimagefilegenerator.h"
#include "dicomdictionary.h"
#include "volume.h"
#include "image.h"
#include "series.h"
#include "study.h"
#include "patient.h"
#include "dicomwriter.h"
#include "dicomvalueattribute.h"
#include "logging.h"

#include <vtkImageData.h>

namespace udg {

DICOMImageFileGenerator::DICOMImageFileGenerator()
{
    m_input = NULL;
}

DICOMImageFileGenerator::~DICOMImageFileGenerator()
{

}

void DICOMImageFileGenerator::setInput(Volume *volume)
{
    m_input = volume;
}

bool DICOMImageFileGenerator::generateDICOMFiles()
{
    Q_ASSERT(m_input);

    // Analitzem el SOP Class UID per saber si sabem generar el tipus de fitxers
    QString sopClass = m_input->getImage(0)->getParentSeries()->getSOPClassUID();

    if (sopClass == UIDSecondaryCaptureImageStorage)
    {
        return generateSecondaryCaptureDICOMFiles();
    }
    else
    {
        DEBUG_LOG("SOP Class UID no suportat: " + sopClass);
        return false;
    }

}

bool DICOMImageFileGenerator::generateSecondaryCaptureDICOMFiles()
{
    DICOMWriter *writer;
    int i = 0;

    foreach (Image *image, m_input->getImages())
    {
        writer = DICOMWriter::newInstance();

        // \TODO
        writer->setPath(m_dir.absolutePath() + "/" + image->getSOPInstanceUID());

        fillPatientInfo(writer, image->getParentSeries()->getParentStudy()->getParentPatient());
        fillStudyInfo(writer, image->getParentSeries()->getParentStudy());
        fillSeriesInfo(writer, image->getParentSeries());
        fillGeneralEquipmentInfo(writer, image->getParentSeries());
        fillGeneralImageInfo(writer, image);
        fillImagePixelInfo(writer, image);
        fillSCInfo(writer, image);
        fillSOPInfo(writer, image);

        // Afegim el pixel data
        DICOMValueAttribute pixelData;
        pixelData.setTag(DICOMPixelData);
        pixelData.setValue(m_input->getImageScalarPointer(i));
        writer->addValueAttribute(&pixelData);

        // \TODO Si falla a l'escriure cal decidir què fer amb els fitxers que prèviament s'han pogut generar. Esborrar-los?
        if (! writer->write())
        {
            return false;
        }

        image->setPath(writer->getPath());
        i++;

        delete writer;
    }

    return true;
}

void DICOMImageFileGenerator::fillGeneralImageInfo(DICOMWriter *writer, Image *image)
{
    DICOMValueAttribute instanceNumber;
    instanceNumber.setTag(DICOMInstanceNumber);
    instanceNumber.setValue(image->getInstanceNumber());
    writer->addValueAttribute(&instanceNumber);

    // Patient Position. Tipus 2C
    DICOMValueAttribute patientOrientation;
    patientOrientation.setTag(DICOMPatientOrientation);
    patientOrientation.setValue(image->getPatientOrientation().getDICOMFormattedPatientOrientation());
    writer->addValueAttribute(&patientOrientation);

}

void DICOMImageFileGenerator::fillImagePixelInfo(DICOMWriter *writer, Image *image)
{
    DICOMValueAttribute samplesPerPixel;
    samplesPerPixel.setTag(DICOMSamplesPerPixel);
    samplesPerPixel.setValue(image->getSamplesPerPixel());
    writer->addValueAttribute(&samplesPerPixel);

    DICOMValueAttribute photometricRepresentation;
    photometricRepresentation.setTag(DICOMPhotometricInterpretation);
    photometricRepresentation.setValue(image->getPhotometricInterpretation());
    writer->addValueAttribute(&photometricRepresentation);

    DICOMValueAttribute rows;
    rows.setTag(DICOMRows);
    rows.setValue(image->getRows());
    writer->addValueAttribute(&rows);

    DICOMValueAttribute columns;
    columns.setTag(DICOMColumns);
    columns.setValue(image->getColumns());
    writer->addValueAttribute(&columns);

    DICOMValueAttribute bitsAllocated;
    bitsAllocated.setTag(DICOMBitsAllocated);
    bitsAllocated.setValue(image->getBitsAllocated());
    writer->addValueAttribute(&bitsAllocated);

    DICOMValueAttribute bitsStored;
    bitsStored.setTag(DICOMBitsStored);
    bitsStored.setValue(image->getBitsStored());
    writer->addValueAttribute(&bitsStored);

    DICOMValueAttribute highBit;
    highBit.setTag(DICOMHighBit);
    highBit.setValue(image->getHighBit());
    writer->addValueAttribute(&highBit);

    DICOMValueAttribute pixelRepresentation;
    pixelRepresentation.setTag(DICOMPixelRepresentation);
    pixelRepresentation.setValue(image->getPixelRepresentation());
    writer->addValueAttribute(&pixelRepresentation);

    // Planar Configuration 1C
    if (image->getSamplesPerPixel() > 1)
    {
        DICOMValueAttribute planarConfiguration;
        planarConfiguration.setTag(DICOMPlanarConfiguration);
        // 0 = R1 G1 B1 R2 G2 B2 ... i 1 = R1 R2 ... G1 G2 ... B1 B2 ...
        planarConfiguration.setValue(0);
        writer->addValueAttribute(&planarConfiguration);
    }

}

void DICOMImageFileGenerator::fillSCInfo(DICOMWriter *writer, Image *image)
{
    Q_UNUSED(image);

    DICOMValueAttribute conversionType;
    conversionType.setTag(DICOMConversionType);
    conversionType.setValue(QString("WSD"));
    writer->addValueAttribute(&conversionType);
}

void DICOMImageFileGenerator::fillSOPInfo(DICOMWriter *writer, Image *image)
{
    DICOMValueAttribute classUID;
    classUID.setTag(DICOMSOPClassUID);
    classUID.setValue(image->getParentSeries()->getSOPClassUID());
    writer->addValueAttribute(&classUID);

    DICOMValueAttribute instanceUID;
    instanceUID.setTag(DICOMSOPInstanceUID);
    instanceUID.setValue(image->getSOPInstanceUID());
    writer->addValueAttribute(&instanceUID);
}

}
