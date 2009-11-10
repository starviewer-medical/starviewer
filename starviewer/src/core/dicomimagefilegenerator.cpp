/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGDICOMIMAGEFILEGENERATOR_CPP
#define UDGDICOMIMAGEFILEGENERATOR_CPP

#include "dicomimagefilegenerator.h"
#include "dicomdictionary.h"
#include "volume.h"
#include "image.h"
#include "series.h"
#include "study.h"
#include "patient.h"
#include "dicomwriter.h"
#include "dicomvalueattribute.h"
#include "dicomtag.h"
#include <vtkImageData.h>

namespace udg {

DICOMImageFileGenerator::DICOMImageFileGenerator()
{
    m_input = NULL;
}


DICOMImageFileGenerator::~DICOMImageFileGenerator()
{

}

void DICOMImageFileGenerator::setInput( Volume * volume )
{
    m_input = volume;
}


bool DICOMImageFileGenerator::generateDICOMFiles()
{
    Q_ASSERT( m_input );

    // Analitzem el SOP Class UID per saber si sabem generar el tipus de fitxers
    QString sopClass = m_input->getImages().at(0)->getParentSeries()->getSOPClassUID();

    if ( sopClass  ==  UID_SecondaryCaptureImageStorage )
    {
        return generateSCDICOMFiles();
    }
    else
    {
        DEBUG_LOG( "SOP Class UID no suportat: " + sopClass );
        return false;
    }

}

bool DICOMImageFileGenerator::generateSCDICOMFiles()
{
    vtkImageData * data = m_input->getVtkData();

    int * dimensions = data->GetDimensions();
    int bytesPerImage = data->GetScalarSize()*data->GetNumberOfScalarComponents()*dimensions[0]*dimensions[1];
    const char * scalarPointer = static_cast<const char *>( data->GetScalarPointer() );
    int i = 1;
    
    DICOMWriter * writer;

    foreach ( Image *image , m_input->getImages() )
    {
        writer = DICOMWriter::newInstance();

        // \TODO
        writer->setPath( m_dir.absolutePath() + "/" + image->getSOPInstanceUID() + ".dcm" );

        fillPatientInfo( writer , image->getParentSeries()->getParentStudy()->getParentPatient() );
        fillStudyInfo( writer , image->getParentSeries()->getParentStudy() );
        fillSeriesInfo( writer , image->getParentSeries() );
        fillGeneralEquipmentInfo( writer , image->getParentSeries() );
        fillGeneralImageInfo( writer, image );
        fillImagePixelInfo( writer, image );
        fillSCInfo( writer, image );
        fillSOPInfo( writer, image );

        //Afegim el pixel data
        DICOMValueAttribute pixelData;
        pixelData.setTag( DICOM_PixelData );
        pixelData.setValue( QByteArray( scalarPointer , bytesPerImage ) );
        writer->addValueAttribute( &pixelData );

        scalarPointer += bytesPerImage;
        i++;

        writer->write();

        image->setPath( writer->getPath() );

    }
}

void DICOMImageFileGenerator::fillGeneralImageInfo( DICOMWriter * writer , Image * image)
{
    DICOMValueAttribute instanceNumber;
    instanceNumber.setTag( DICOM_InstanceNumber );
    instanceNumber.setValue( image->getInstanceNumber() );
    writer->addValueAttribute( &instanceNumber );

    // Patient Position. Tipus 2C
    DICOMValueAttribute patientOrientation;
    patientOrientation.setTag( DICOM_PatientOrientation );
    patientOrientation.setValue( image->getPatientOrientation() );
    writer->addValueAttribute( &patientOrientation );

}

void DICOMImageFileGenerator::fillImagePixelInfo( DICOMWriter * writer , Image * image)
{
    DICOMValueAttribute samplesPerPixel;
    samplesPerPixel.setTag( DICOM_SamplesPerPixel );
    samplesPerPixel.setValue( image->getSamplesPerPixel() );
    writer->addValueAttribute( &samplesPerPixel );

    DICOMValueAttribute photometricRepresentation;
    photometricRepresentation.setTag( DICOM_PhotometricInterpretation );
    photometricRepresentation.setValue( image->getPhotometricInterpretation() );
    writer->addValueAttribute( &photometricRepresentation );

    DICOMValueAttribute rows;
    rows.setTag( DICOM_Rows );
    rows.setValue( image->getRows() );
    writer->addValueAttribute( &rows );

    DICOMValueAttribute columns;
    columns.setTag( DICOM_Columns );
    columns.setValue( image->getColumns() );
    writer->addValueAttribute( &columns );

    DICOMValueAttribute bitsAllocated;
    bitsAllocated.setTag( DICOM_BitsAllocated );
    bitsAllocated.setValue( image->getBitsAllocated() );
    writer->addValueAttribute( &bitsAllocated );

    DICOMValueAttribute bitsStored;
    bitsStored.setTag( DICOM_BitsStored );
    bitsStored.setValue( image->getBitsStored() );
    writer->addValueAttribute( &bitsStored );

    DICOMValueAttribute highBit;
    highBit.setTag( DICOM_HighBit);
    highBit.setValue( image->getHighBit() );
    writer->addValueAttribute( &highBit );

    DICOMValueAttribute pixelRepresentation;
    pixelRepresentation.setTag( DICOM_PixelRepresentation );
    pixelRepresentation.setValue( image->getPixelRepresentation() );
    writer->addValueAttribute( &pixelRepresentation );


    // Planar Configuration 1C
    if ( image->getSamplesPerPixel() > 1)
    {
        DICOMValueAttribute planarConfiguration;
        planarConfiguration.setTag( DICOM_PlanarConfiguration );
        planarConfiguration.setValue( 0 ); // 0 = R1 G1 B1 R2 G2 B2 ... i 1 = R1 R2 ... G1 G2 ... B1 B2 ...
        writer->addValueAttribute( &planarConfiguration );
    }

}

void DICOMImageFileGenerator::fillSCInfo( DICOMWriter * writer , Image * image )
{
    Q_UNUSED( image );

    DICOMValueAttribute conversionType;
    conversionType.setTag( DICOM_ConversionType );
    conversionType.setValue( QString("WSD") );
    writer->addValueAttribute( &conversionType );
}

void DICOMImageFileGenerator::fillSOPInfo( DICOMWriter * writer , Image * image )
{
        DICOMValueAttribute classUID;
        classUID.setTag( DICOM_SOPClassUID );
        classUID.setValue( image->getParentSeries()->getSOPClassUID() );
        writer->addValueAttribute( &classUID );

        DICOMValueAttribute instanceUID;
        instanceUID.setTag( DICOM_SOPInstanceUID );
        instanceUID.setValue( image->getSOPInstanceUID() );
        writer->addValueAttribute( &instanceUID );
}

}

#endif
