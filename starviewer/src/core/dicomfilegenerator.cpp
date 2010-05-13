/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGDICOMFILEGENERATOR_CPP
#define UDGDICOMFILEGENERATOR_CPP

#include "dicomfilegenerator.h"

#include "dicomwriter.h"
#include "patient.h"
#include "study.h"
#include "series.h"
#include "dicomdictionary.h"
#include "dicomvalueattribute.h"

namespace udg {

DICOMFileGenerator::DICOMFileGenerator()
{
}

DICOMFileGenerator::~DICOMFileGenerator()
{
}

bool DICOMFileGenerator::setDirPath( QString path )
{
    QDir dir( path );

    if ( !dir.exists() )
    {
        if ( !dir.mkpath( dir.path() ) )
            return false;
    }
    else
    {
        if ( !dir.isReadable() )
            return false;
    }

    m_dir = dir;

    return true;
}


void DICOMFileGenerator::fillPatientInfo( DICOMWriter *writer, Patient *patient )
{
    DICOMValueAttribute patientsName;
    patientsName.setTag( DICOMPatientsName );
    patientsName.setValue( patient->getFullName() );
    writer->addValueAttribute( &patientsName );

    DICOMValueAttribute patientID;
    patientID.setTag( DICOMPatientID );
    patientID.setValue( patient->getID() );
    writer->addValueAttribute( &patientID );

    DICOMValueAttribute patientBirthDate;
    patientBirthDate.setTag( DICOMPatientsBirthDate );
    patientBirthDate.setValue( patient->getBirthDate() );
    writer->addValueAttribute( &patientBirthDate );

    DICOMValueAttribute patientSex;
    patientSex.setTag( DICOMPatientsSex );
    patientSex.setValue( patient->getSex() );
    writer->addValueAttribute( &patientSex );

}

void DICOMFileGenerator::fillStudyInfo( DICOMWriter *writer, Study *study )
{
    DICOMValueAttribute studyInstanceUID;
    studyInstanceUID.setTag( DICOMStudyInstanceUID );
    studyInstanceUID.setValue( study->getInstanceUID() );
    writer->addValueAttribute( &studyInstanceUID );

    DICOMValueAttribute studyDate;
    studyDate.setTag( DICOMStudyDate );
    studyDate.setValue( study->getDate() );
    writer->addValueAttribute( &studyDate );

    DICOMValueAttribute studyTime;
    studyTime.setTag( DICOMStudyTime );
    studyTime.setValue( study->getTime() );
    writer->addValueAttribute( &studyTime );

    DICOMValueAttribute referringName;
    referringName.setTag( DICOMReferringPhysiciansName );
    referringName.setValue( study->getReferringPhysiciansName() );
    writer->addValueAttribute( &referringName );

    DICOMValueAttribute studyID;
    studyID.setTag( DICOMStudyID );
    studyID.setValue( study->getID() );
    writer->addValueAttribute( &studyID );

    DICOMValueAttribute accessionNumber;
    accessionNumber.setTag( DICOMAccessionNumber );
    accessionNumber.setValue( study->getAccessionNumber() );
    writer->addValueAttribute( &accessionNumber );

    if ( !study->getDescription().isEmpty() ) // Tipus 3
    {
        DICOMValueAttribute studyDescription;
        studyDescription.setTag( DICOMStudyDescription );
        studyDescription.setValue( study->getDescription() );
        writer->addValueAttribute( &studyDescription );
    }

    if ( !study->getPatientAge().isEmpty() ) // Tipus 3
    {
        DICOMValueAttribute patientAge;
        patientAge.setTag( DICOMPatientsAge );
        patientAge.setValue( study->getPatientAge() );
        writer->addValueAttribute( &patientAge );
    }

//    if ( !study->getHeight() > 0.0 )
//    {
//        DICOMValueAttribute patientHeight;
//        patientHeight.setTag( DICOMPatientsSize );
//        patientHeight.setValue( study->getHeight() );
//        writer->addValueAttribute( &patientHeight );
//    }
//
//    if ( !study->getWeight() > 0.0 )
//    {
//        DICOMValueAttribute patientWeight;
//        patientWeight.setTag( DICOMPatientsWeight );
//        patientWeight.setValue( study->getWeight() );
//        writer->addValueAttribute( &patientWeight );
//    }

}


void DICOMFileGenerator::fillSeriesInfo( DICOMWriter *writer, Series *series )
{
    DICOMValueAttribute modality;
    modality.setTag( DICOMModality );
    modality.setValue( series->getModality() );
    writer->addValueAttribute( &modality );

    DICOMValueAttribute instanceUID;
    instanceUID.setTag( DICOMSeriesInstanceUID );
    instanceUID.setValue( series->getInstanceUID() );
    writer->addValueAttribute( &instanceUID );

    DICOMValueAttribute seriesNumber;
    seriesNumber.setTag( DICOMSeriesNumber );
    seriesNumber.setValue( series->getSeriesNumber() );
    writer->addValueAttribute( &seriesNumber );

    // \TODO Laterality Tipus 2C
    DICOMValueAttribute laterality;
    laterality.setTag( DICOMLaterality );
    laterality.setValue( QString("") );
    writer->addValueAttribute( &laterality );

    if ( series->getDate().isValid() ) // Tipus 3
    {
        DICOMValueAttribute seriesDate;
        seriesDate.setTag( DICOMSeriesDate );
        seriesDate.setValue( series->getDate() );
        writer->addValueAttribute( &seriesDate );
    }

    if ( series->getTime().isValid() ) // Tipus 3
    {
        DICOMValueAttribute seriesTime;
        seriesTime.setTag( DICOMSeriesTime );
        seriesTime.setValue( series->getTime() );
        writer->addValueAttribute( &seriesTime );
    }

    if ( !series->getProtocolName().isEmpty() ) // Tipus 3
    {
        DICOMValueAttribute protocolName;
        protocolName.setTag( DICOMProtocolName );
        protocolName.setValue( series->getProtocolName() );
        writer->addValueAttribute( &protocolName );
    }

    if ( !series->getDescription().isEmpty() ) // Tipus 3
    {
        DICOMValueAttribute seriesDescription;
        seriesDescription.setTag( DICOMSeriesDescription );
        seriesDescription.setValue( series->getDescription() );
        writer->addValueAttribute( &seriesDescription );
    }

    // Patient Position (Tipus 2C) El deixem per casos especificis


}


void DICOMFileGenerator::fillGeneralEquipmentInfo( DICOMWriter *writer, Series *series )
{
    DICOMValueAttribute manufacturer;
    manufacturer.setTag( DICOMManufacturer );
    manufacturer.setValue( series->getManufacturer() );
    writer->addValueAttribute( &manufacturer );

    if ( !series->getInstitutionName().isEmpty() ) // Tipus 3
    {
        DICOMValueAttribute institutionName;
        institutionName.setTag( DICOMInstitutionName );
        institutionName.setValue( series->getInstitutionName() );
        writer->addValueAttribute( &institutionName );
    }
}


}

#endif
