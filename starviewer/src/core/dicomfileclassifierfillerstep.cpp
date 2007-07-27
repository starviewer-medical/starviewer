/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "dicomfileclassifierfillerstep.h"
#include "logging.h"
#include "dicomtagreader.h"
#include "patientfillerinput.h"
#include "patient.h"
#include "study.h"
#include "series.h"
#include "image.h"

namespace udg {

DICOMFileClassifierFillerStep::DICOMFileClassifierFillerStep()
 : PatientFillerStep()
{
    m_dicomReader = new DICOMTagReader;
}

DICOMFileClassifierFillerStep::~DICOMFileClassifierFillerStep()
{
}

bool DICOMFileClassifierFillerStep::fill()
{
    bool ok = false;
    // processarem cadascun dels arxius de l'input i els anirem col·locant a Patient
    if( m_input )
    {
        QStringList fileList = m_input->getFilesList();
        foreach( QString file, fileList )
        {
            classifyFile( file );
        }
    }
    else
        DEBUG_LOG("No tenim input!");

    return ok;
}

bool DICOMFileClassifierFillerStep::classifyFile( QString file )
{
    bool ok = false;

    ok = m_dicomReader->setFile( file );
    if( ok )
    {
        // primer recopilem tota la informació que ens permet ubicar l'arxiu dins de l'estructura
        QString patientName = m_dicomReader->getAttributeByName( DCM_PatientsName );
        QString patientID = m_dicomReader->getAttributeByName( DCM_PatientID );
        QString studyUID = m_dicomReader->getAttributeByName( DCM_StudyInstanceUID );
        QString seriesUID = m_dicomReader->getAttributeByName( DCM_SeriesInstanceUID );
        QString sopInstanceUID = m_dicomReader->getAttributeByName( DCM_SOPInstanceUID );

        Patient *patient = getPatient( patientName, patientID );
        if( !patient )
        {
            patient = createPatient();
            m_input->addPatient( patient );
        }

        Study *study = patient->getStudy( studyUID );
        if( !study )
        {
            study = createStudy();
            patient->addStudy( study );
        }

        Series *series = study->getSeries( seriesUID );
        if( !series )
        {
            series = createSeries();
            study->addSeries( series );
        }

        // Podrem tenir o bé Images, o bé KINs o bé PresentationStates
        if( isImageSeries(series) )
        {
            Image *image = series->getImage( sopInstanceUID );
            if( !image )
            {
                image = createImage();
                image->setPath( file );
                series->addImage( image );
            }
        }
        else if( isKeyImageNoteSeries(series) )
        {
            // TODO crear objectes KIN
            DEBUG_LOG("Cal crear objectes KIN");
        }
        else if( isPresentationStateSeries(series) )
        {
            // TODO crear objectes PresentationState
            DEBUG_LOG("Cal crear objectes Presentation State");
        }
        else
        {
            // TODO tipu de Sèrie no suportat/no sabem classificar
            DEBUG_LOG("tipu de serie no suportat/que no sabem classificar");
        }

    }

    return ok;
}

Patient *DICOMFileClassifierFillerStep::getPatient( QString patientName, QString patientID )
{
    Patient *patient = m_input->getPatientByName( patientName );
    if( patient )
        return patient;
    else
        return m_input->getPatientByID( patientID );
}

Patient *DICOMFileClassifierFillerStep::createPatient()
{
    Patient *patient = new Patient;

    patient->setFullName( m_dicomReader->getAttributeByName( DCM_PatientsName ) );
    patient->setID( m_dicomReader->getAttributeByName( DCM_PatientID ) );

    QString value = m_dicomReader->getAttributeByName( DCM_PatientsBirthDate );
    if( !value.isEmpty() )
    {
        patient->setBirthDate( value.mid(0,4).toInt(), value.mid(4,2).toInt(), value.mid(6,2).toInt() );
    }

    patient->setSex( m_dicomReader->getAttributeByName( DCM_PatientsSex ) );

    return patient;
}

Study *DICOMFileClassifierFillerStep::createStudy()
{
    Study *study = new Study;

    study->setInstanceUID( m_dicomReader->getAttributeByName( DCM_StudyInstanceUID ) );

    QString value = m_dicomReader->getAttributeByName( DCM_StudyDate );
    if( !value.isEmpty() )
        study->setDate( value.mid(0,4).toInt(), value.mid(4,2).toInt(), value.mid(6,2).toInt() );

    value = m_dicomReader->getAttributeByName( DCM_StudyTime );
    if( !value.isEmpty() )
        study->setTime( value.mid(0,2).toInt(), value.mid(2,2).toInt() ); // precissió d'hores i minuts

    study->setID( m_dicomReader->getAttributeByName( DCM_StudyID ) );
    study->setAccessionNumber( m_dicomReader->getAttributeByName( DCM_AccessionNumber ) );
    study->setDescription( m_dicomReader->getAttributeByName( DCM_StudyDescription ) );
    study->setPatientAge( m_dicomReader->getAttributeByName( DCM_PatientsAge ).toInt() );
    study->setHeight( m_dicomReader->getAttributeByName( DCM_PatientsSize ).toDouble() );
    study->setWeight( m_dicomReader->getAttributeByName( DCM_PatientsWeight ).toDouble() );

    return study;
}

Series *DICOMFileClassifierFillerStep::createSeries()
{
    Series *series = new Series;

    series->setInstanceUID( m_dicomReader->getAttributeByName( DCM_StudyInstanceUID ) );
    series->setModality( m_dicomReader->getAttributeByName( DCM_Modality ) );
    series->setSeriesNumber( m_dicomReader->getAttributeByName( DCM_SeriesNumber ) );

    QString value = m_dicomReader->getAttributeByName( DCM_SeriesDate );
    if( !value.isEmpty() )
        series->setDate( value.mid(0,4).toInt(), value.mid(4,2).toInt(), value.mid(6,2).toInt() );
    value = m_dicomReader->getAttributeByName( DCM_SeriesTime );
    if( !value.isEmpty() )
        series->setTime( value.mid(0,2).toInt(), value.mid(2,2).toInt() );

    series->setInstitutionName( m_dicomReader->getAttributeByName( DCM_InstitutionName ) );
    series->setPatientPosition( m_dicomReader->getAttributeByName( DCM_PatientPosition ) );
    series->setProtocolName( m_dicomReader->getAttributeByName( DCM_ProtocolName ) );
    series->setDescription( m_dicomReader->getAttributeByName( DCM_SeriesDescription ) );
    series->setFrameOfReferenceUID( m_dicomReader->getAttributeByName( DCM_FrameOfReferenceUID ) );
    series->setPositionReferenceIndicator( m_dicomReader->getAttributeByName( DCM_PositionReferenceIndicator ) );

    return series;
}

Image *DICOMFileClassifierFillerStep::createImage()
{
    Image *image = new Image;

    image->setSOPInstanceUID( m_dicomReader->getAttributeByName( DCM_SOPInstanceUID ) );
    image->setInstanceNumber( m_dicomReader->getAttributeByName( DCM_InstanceNumber ) );
    image->setPatientOrientation( m_dicomReader->getAttributeByName( DCM_PatientOrientation ) );

    QString value = m_dicomReader->getAttributeByName( DCM_ContentDate );
    if( !value.isEmpty() )
        image->setContentDate( value.mid(0,4).toInt(), value.mid(4,2).toInt(), value.mid(6,2).toInt() );
    value = m_dicomReader->getAttributeByName( DCM_ContentTime );
    if( !value.isEmpty() )
        image->setContentTime( value.mid(0,2).toInt(), value.mid(2,2).toInt() );

    image->setImagesInAcquisition( m_dicomReader->getAttributeByName( DCM_ImagesInAcquisition ).toInt() );
    image->setComments( m_dicomReader->getAttributeByName( DCM_ImageComments ) );

    value = m_dicomReader->getAttributeByName( DCM_ImageOrientationPatient );

    QStringList list = value.split( "\\" );
    if( list.size() == 6 )
    {
        double orientation[6];
        for( int i = 0; i < 6; i++ )
            orientation[ i ] = list.at( i ).toDouble();

        image->setImageOrientation( orientation );
    }

    value = m_dicomReader->getAttributeByName( DCM_ImagePosition );
    list = value.split("\\");
    if( list.size() == 3 )
    {
        double position[3] = { list.at(0).toDouble(), list.at(1).toDouble(), list.at(2).toDouble() };
        image->setImagePosition( position );
    }

    image->setSamplesPerPixel( m_dicomReader->getAttributeByName( DCM_SamplesPerPixel ).toInt() );
    image->setPhotometricInterpretation( m_dicomReader->getAttributeByName( DCM_PhotometricInterpretation ).toInt() );
    image->setRows( m_dicomReader->getAttributeByName( DCM_Rows ).toInt() );
    image->setColumns( m_dicomReader->getAttributeByName( DCM_Columns ).toInt() );

    return image;
}

bool DICOMFileClassifierFillerStep::isImageSeries( Series *series )
{
    // TODO aquí caldria especificar quines són les modalitats que acceptem com a imatges
    QStringList supportedModalitiesAsImage;
    supportedModalitiesAsImage << "CT" << "MR" << "US" << "RT" << "DX" << "MG";
    return supportedModalitiesAsImage.contains( series->getModality() );
}

bool DICOMFileClassifierFillerStep::isKeyImageNoteSeries( Series *series )
{
    return series->getModality() == "KO";
}

bool DICOMFileClassifierFillerStep::isPresentationStateSeries( Series *series )
{
    return series->getModality() == "PR";
}

}
