/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "qdicomdumpmammographywidget.h"
#include "image.h"
#include "dicomtagreader.h"
#include "dicomdictionary.h"

namespace udg {

QDicomDumpMammographyWidget::QDicomDumpMammographyWidget( QWidget *parent )
 : QWidget(parent)
{
    setupUi( this );
    initializeLabels();
}

QDicomDumpMammographyWidget::~QDicomDumpMammographyWidget()
{
}

void QDicomDumpMammographyWidget::setCurrentDisplayedImage( Image *currentImage )
{
    if ( currentImage != NULL)
        dumpDICOMInformation( currentImage );
}

void QDicomDumpMammographyWidget::initializeLabels()
{
    /*
    Patient information: 
        patient’s name(OK)
        patient ID(OK)
        patient’s age(OK)
        birth date(OK)
    */
    // Study, series & image information
    // TODO Caldria comprovar si l'acquisition date és el mateix que DICOMContentDate/DICOMContentTime
    // En la informació genèrica del dump fem servir el "Content". Si aquest ens val, no caldrà obtenir el "Acquisition"
    // const DICOMTag    DICOMAcquisitionDate(0x0008, 0x0022); ->alt. const DICOMTag    DICOMContentDate(0x0008, 0x0023);
    // const DICOMTag    DICOMAcquisitionDatetime(0x0008, 0x002a); ->alt. const DICOMTag    DICOMContentTime(0x0008, 0x0033);
    m_operatorsNameValueLabel->setText( "-" );
    m_compressionForceValueLabel->setText( "-" );
    m_bodyPartThicknessValueLabel->setText( "-" );
    m_positionerPrimaryAngleValueLabel->setText( "-" );

    // Equipment information
    m_manufacturerValueLabel->setText( "-" );
    m_institutionNameValueLabel->setText( "-" );
    m_institutionAddressValueLabel->setText( "-" );
    m_manufacturersModelNameValueLabel->setText( "-" );
    m_deviceSerialNumberValueLabel->setText( "-" );
    m_detectorIDValueLabel->setText( "-" );
    m_softwareVersionsValueLabel->setText( "-" );

    // X-Ray acquisition dose information
    m_kiloVoltagePeakValueLabel->setText( "-" );
    m_exposureValueLabel->setText( "-" );
    m_exposureTimeValueLabel->setText( "-" );
    m_filterMaterialValueLabel->setText( "-" );
    m_anodeTargetMaterialValueLabel->setText( "-" );
    m_relativeExposureValueLabel->setText( "-" );
    m_organDoseValueLabel->setText( "-" );
    m_entranceDoseValueLabel->setText( "-" );
}

void QDicomDumpMammographyWidget::dumpDICOMInformation( Image *currentImage )
{
    DICOMTagReader dicomReader;
    bool ok = dicomReader.setFile( currentImage->getPath() );
    if( ok )
    {
        //
        // Study, series & image information
        //
        // TODO aquest tag es podria arribar a incloure com a genèric
        if( dicomReader.tagExists( DICOMOperatorsName ) )
            m_operatorsNameValueLabel->setText( dicomReader.getAttributeByName( DICOMOperatorsName ) );
        else
            m_operatorsNameValueLabel->setText("N/A");

        if( dicomReader.tagExists( DICOMCompressionForce ) )
            m_compressionForceValueLabel->setText( dicomReader.getAttributeByName( DICOMCompressionForce ) );
        else
            m_compressionForceValueLabel->setText("N/A");

        if( dicomReader.tagExists( DICOMBodyPartThickness ) )
            m_bodyPartThicknessValueLabel->setText( dicomReader.getAttributeByName( DICOMBodyPartThickness ) );
        else
            m_bodyPartThicknessValueLabel->setText("N/A");

        if( dicomReader.tagExists( DICOMPositionerPrimaryAngle ) )
            m_positionerPrimaryAngleValueLabel->setText( dicomReader.getAttributeByName( DICOMPositionerPrimaryAngle ) );
        else
            m_positionerPrimaryAngleValueLabel->setText("N/A");
                
        //
        // Equipment information
        //
        if( dicomReader.tagExists( DICOMManufacturer ) )
            m_manufacturerValueLabel->setText( dicomReader.getAttributeByName( DICOMManufacturer ) );
        else
            m_manufacturerValueLabel->setText("N/A");

        if( dicomReader.tagExists( DICOMInstitutionName ) )
            m_institutionNameValueLabel->setText( dicomReader.getAttributeByName( DICOMInstitutionName ) );
        else
            m_institutionNameValueLabel->setText("N/A");

        if( dicomReader.tagExists( DICOMInstitutionAddress ) )
            m_institutionAddressValueLabel->setText( dicomReader.getAttributeByName( DICOMInstitutionAddress ) );
        else
            m_institutionAddressValueLabel->setText("N/A");

        if( dicomReader.tagExists( DICOMManufacturersModelName ) )
            m_manufacturersModelNameValueLabel->setText( dicomReader.getAttributeByName( DICOMManufacturersModelName ) );
        else
            m_manufacturersModelNameValueLabel->setText("N/A");

        if( dicomReader.tagExists( DICOMDeviceSerialNumber ) )
            m_deviceSerialNumberValueLabel->setText( dicomReader.getAttributeByName( DICOMDeviceSerialNumber ) );
        else
            m_deviceSerialNumberValueLabel->setText("N/A");

        if( dicomReader.tagExists( DICOMDetectorID ) )
            m_detectorIDValueLabel->setText( dicomReader.getAttributeByName( DICOMDetectorID ) );
        else
            m_detectorIDValueLabel->setText("N/A");

        if( dicomReader.tagExists( DICOMSoftwareVersions ) )
            m_softwareVersionsValueLabel->setText( dicomReader.getAttributeByName( DICOMSoftwareVersions ) );
        else
            m_softwareVersionsValueLabel->setText("N/A");

        //
        // X-Ray acquisition dose information
        //
        if( dicomReader.tagExists( DICOMKVP ) )
            m_kiloVoltagePeakValueLabel->setText( dicomReader.getAttributeByName( DICOMKVP ) );
        else
            m_kiloVoltagePeakValueLabel->setText("N/A");

        // L'exposure l'obtenim en miliAmpers/segon(mAs). Si els volguéssim en microAmpers/segon, caldria fer servir el tag DICOMExposureInMicroAs
        if( dicomReader.tagExists( DICOMExposure ) )
            m_exposureValueLabel->setText( dicomReader.getAttributeByName( DICOMExposure ) );
        else
            m_exposureValueLabel->setText("N/A");

        if( dicomReader.tagExists( DICOMExposureTime ) )
            m_exposureTimeValueLabel->setText( dicomReader.getAttributeByName( DICOMExposureTime ) );
        else
            m_exposureTimeValueLabel->setText("N/A");
    
        if( dicomReader.tagExists( DICOMFilterMaterial ) )
            m_filterMaterialValueLabel->setText( dicomReader.getAttributeByName( DICOMFilterMaterial ) );
        else
            m_filterMaterialValueLabel->setText("N/A");
    
        if( dicomReader.tagExists( DICOMAnodeTargetMaterial ) )
            m_anodeTargetMaterialValueLabel->setText( dicomReader.getAttributeByName( DICOMAnodeTargetMaterial ) );
        else
            m_anodeTargetMaterialValueLabel->setText("N/A");
    
        if( dicomReader.tagExists( DICOMRelativeXRayExposure ) )
            m_relativeExposureValueLabel->setText( dicomReader.getAttributeByName( DICOMRelativeXRayExposure ) );
        else
            m_relativeExposureValueLabel->setText("N/A");
    
        // Se suposa que les unitats són "mGy"
        if( dicomReader.tagExists( DICOMOrganDose ) )
            m_organDoseValueLabel->setText( dicomReader.getAttributeByName( DICOMOrganDose ) );
        else
            m_organDoseValueLabel->setText("N/A");
    
        if( dicomReader.tagExists( DICOMEntranceDoseInmGy ) )
            m_entranceDoseValueLabel->setText( dicomReader.getAttributeByName( DICOMEntranceDoseInmGy ) );
        else
            m_entranceDoseValueLabel->setText("N/A");
    }
}

}
