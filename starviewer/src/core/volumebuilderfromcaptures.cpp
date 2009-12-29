/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGVOLUMEBUILDERFROMCAPTURES_CPP
#define UDGVOLUMEBUILDERFROMCAPTURES_CPP

#include "volumebuilderfromcaptures.h"

#include <vtkImageAppend.h>
#include <vtkImageData.h>
#include <vtkImageFlip.h>

#include "volume.h"
#include "series.h"
#include "study.h"
#include "image.h"

#include "dcmtk/dcmdata/dcuid.h"

namespace udg {

VolumeBuilderFromCaptures::VolumeBuilderFromCaptures()
{
    m_vtkImageAppend = vtkImageAppend::New();
    m_vtkImageAppend->SetAppendAxis(2);

    this->setSeriesDescription( QString("Generated from screen captures") );
    m_modality = QString("OT");
}

VolumeBuilderFromCaptures::~VolumeBuilderFromCaptures()
{
    m_vtkImageAppend->Delete();
}

void VolumeBuilderFromCaptures::addCapture( vtkImageData *data )
{
    // \TODO Realitzem alguna comprovació o deixem que el vtkImageAppend es queixi?
    m_vtkImageAppend->AddInput( data );
}


void VolumeBuilderFromCaptures::setParentStudy( Study * study )
{
    m_parentStudy = study;
}

bool VolumeBuilderFromCaptures::setModality( QString modality )
{
    // Llista de modalitats vàlides (DICOM PS 3.3 C.7.3.1.1.1).
    QStringList allowedModalities;
    allowedModalities << "CR" << "CT" << "MR" << "US" << "BI" << "DD" << "ES" << "PT" << "ST" << "XA" << "RTIMAGE" << "RTSTRUCT" << "RTRECORD" << "DX" << "IO" << "GM" << "XC" << "AU" << "EPS" << "SR" << "OP" << "NM" << "OT" << "CD" << "DG" << "LS" << "RG" << "TG" << "RF" << "RTDOSE" << "RTPLAN" << "HC" << "MG" << "PX" << "SM" << "PR" << "ECG" << "HD" << "IVUS" << "SMR";

    if ( allowedModalities.contains( modality ) )
    {
        m_modality = modality;
        return true;
    }
    else
    {
        return false;
    }
}

Volume * VolumeBuilderFromCaptures::build()
{
    Q_ASSERT( m_parentStudy );
    Q_ASSERT( m_vtkImageAppend->GetNumberOfInputs() );

    //Creem la nova sèrie
    Series * newSeries = new Series();

    //Omplim la informació de la sèrie a partir de la sèrie de referència

    //Assignem la modalitat segons el valor introduit. El valor per defecte és 'OT' (Other).
    newSeries->setModality( m_modality );
    newSeries->setSOPClassUID( QString(UID_SecondaryCaptureImageStorage) );

    // Generem el SeriesInstanceUID a partir del generador de DCMTK. \TODO Utilitzar el nostre UID_ROOT?
    char seriesUid[100];
    dcmGenerateUniqueIdentifier(seriesUid, SITE_SERIES_UID_ROOT);
    newSeries->setInstanceUID( QString(seriesUid) );
    // \TODO Quin criteri volem seguir per donar nous noms?
    newSeries->setSeriesNumber( QString("0000")+QString::number(m_parentStudy->getSeries().count()) );
    newSeries->setDescription( this->getSeriesDescription() );

     //Assignem la sèrie a l'estudi al qual partenyia l'inputVolume.
    newSeries->setParentStudy( m_parentStudy );
    m_parentStudy->addSeries( newSeries );

    //Obtenim el nou vtkImageData a partir de la sortida del vtkImageAppend.
    //Fem un flip horitzontal per tal utilitzar el mateix sistema de coordenades que DICOM.
    m_vtkImageAppend->Update();

    vtkImageFlip * imageFlip = vtkImageFlip::New();
    imageFlip->SetInputConnection( m_vtkImageAppend->GetOutputPort() );
    imageFlip->SetFilteredAxis(1); //Flip horitzontal
    imageFlip->Update();

    vtkImageData * newVtkData = vtkImageData::New();
    newVtkData->ShallowCopy( imageFlip->GetOutput() );

    imageFlip->Delete();

    //Creem el nou volume
    Volume * newVolume = new Volume();
    newSeries->addVolume( newVolume );

    //Generem les noves imatges a partir del vtkData generat per vtkImageAppend
    int samplesPerPixel = newVtkData->GetNumberOfScalarComponents();
    int bitsAllocated;
    int bitsStored;
    int highBit;
    int pixelRepresentation;
    int rows;
    int columns;

    // \TODO Potser podriem ser mes precisos
    QString photometricInterpretation;
    if ( samplesPerPixel == 1 )
    {
        photometricInterpretation = QString("MONOCHROME2");
    }
    else if ( samplesPerPixel == 3 )
    {
        photometricInterpretation = QString("RGB");
    }

    int scalarType = newVtkData->GetScalarType();

    switch ( scalarType )
    {
        //case VTK_CHAR:
        //break;
        case VTK_SIGNED_CHAR:
            bitsAllocated = 8;
            pixelRepresentation = 1;
            break;
        case VTK_UNSIGNED_CHAR:
            bitsAllocated = 8;
            pixelRepresentation = 0;
            break;
        case VTK_SHORT:
            bitsAllocated = 16;
            pixelRepresentation = 1;
            break;
        case VTK_UNSIGNED_SHORT:
            bitsAllocated = 16;
            pixelRepresentation = 0;
            break;
        case VTK_INT:
            bitsAllocated = 32;
            pixelRepresentation = 1;
            break;
        case VTK_UNSIGNED_INT:
            bitsAllocated = 32;
            pixelRepresentation = 0;
            break;
//        case VTK_FLOAT:
//            bitsAllocated = 32;
//            pixelRepresentation = 1; ?
//            break;
//        case VTK_DOUBLE:
//            bitsAllocated = 64;
//            pixelRepresentation = 1; ?
//            break;
        default:
            DEBUG_LOG( QString("Pixel Type no suportat: ") + newVtkData->GetScalarTypeAsString() );

    }

    bitsStored = bitsAllocated;
    highBit = bitsStored - 1;

    int * dimensions = newVtkData->GetDimensions();
    double * spacing = newVtkData->GetSpacing();

    rows = dimensions[1];
    columns = dimensions[0];

    Image * currentImage;

    for ( int i = 0 ; i < dimensions[2] ; i++ )
    {
        currentImage = new Image();

        // Generem el SOPInstanceUID a partir del generador de DCMTK. \TODO Utilitzar el nostre UID_ROOT?
        char instanceUid[100];
        dcmGenerateUniqueIdentifier(instanceUid, SITE_INSTANCE_UID_ROOT);
        currentImage->setSOPInstanceUID( QString(instanceUid) );
        newSeries->addImage( currentImage );
        newVolume->addImage( currentImage );
        currentImage->setParentSeries( newSeries );

        currentImage->setBitsAllocated( bitsAllocated );
        currentImage->setBitsStored( bitsStored );
        currentImage->setHighBit( highBit );
        currentImage->setColumns( columns );
        currentImage->setInstanceNumber( QString::number(i+1) );
        currentImage->setNumberOfFrames( 1 );
        currentImage->setPhotometricInterpretation( photometricInterpretation );
        currentImage->setPixelRepresentation( pixelRepresentation );
        currentImage->setPixelSpacing( spacing[0], spacing[1] );
        currentImage->setRows( rows );
        currentImage->setSamplesPerPixel( samplesPerPixel );

    }

    // Es fa després d'haver inserit les imatges perquè el nou Volume activi el flag de dades carregades.
    newVolume->setData( newVtkData );

    newSeries->setNumberOfPhases( 1 );
    newSeries->setNumberOfSlicesPerPhase( newSeries->getImages().count() );

    newVolume->setNumberOfPhases( 1 );
    newVolume->setNumberOfSlicesPerPhase( newSeries->getNumberOfSlicesPerPhase() );

    //Informació de DEBUG
    DEBUG_LOG( QString("\nNova sèrie generada:") +
     QString("\n  SeriesInstanceUID: ") + newSeries->getInstanceUID() +
     QString("\n  SeriesNumber: ") + newSeries->getSeriesNumber() +
     QString("\n  SeriesDescription: ") + newSeries->getDescription() +
     QString("\n  SOPClassUID: ") + newSeries->getSOPClassUID() +
     QString("\n  Modality: ") + newSeries->getModality() +
     QString("\n  SamplesPerPixel: ") + QString::number(samplesPerPixel) +
     QString("\n  PhotometricInterpretation: ") + photometricInterpretation +
     QString("\n  BitsAllocated: ") + QString::number(bitsAllocated) +
     QString("\n  BitsStored: ") + QString::number(bitsStored) +
     QString("\n  HighBit: ") + QString::number(highBit) +
     QString("\n  PixelRepresentation: ") + QString::number(pixelRepresentation) +
     QString("\n  PixelSpacing: ") + QString::number(spacing[0]) + QString(",") + QString::number(spacing[1]) +
     QString("\n  Num.Imatges: ") + QString::number(newSeries->getImages().count()) );

    return newVolume;

}

}

#endif
