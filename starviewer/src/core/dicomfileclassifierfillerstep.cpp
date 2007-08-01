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
        ok = true;
        QStringList fileList = m_input->getFilesList();
        foreach( QString file, fileList )
        {
            classifyFile( file );
        }
        m_input->addLabel("DICOMFileClassifierFillerStep");
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

        // fem una classificació top-down. Comencem mirant a quin pacient pertany,després estudi, serie fins arribar al nivell
        // d'imatge/kin/PS. TODO potser seria més eficient començar directament per imatge? En cas de descartar aniríem més
        // ràpid o no? o és ben igual?
        // obtenim el pacient si ja existeix, altrament el creem
        Patient *patient = getPatient( patientName, patientID );
        if( !patient )
        {
            patient = createPatient();
            m_input->addPatient( patient );
        }

        // obtenim l'estudi corresponent si ja existeix, altrament el creem
        Study *study = patient->getStudy( studyUID );
        if( !study )
        {
            study = createStudy();
            patient->addStudy( study );
        }

        // obtenim la serie corresponent si ja existeix, altrament la creem
        Series *series = study->getSeries( seriesUID );
        if( !series )
        {
            series = createSeries();
            study->addSeries( series );
        }
        // \TODO Caldria acabar de veure si aquí és el lloc més adient. Seria lògic fer-ho en el queryscreen? Aquí s'hauria de tornar a fer?
        series->addFilePath( file );
    }
    else
    {
        DEBUG_LOG("Error en llegir l'arxiu: " + file );
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

}
