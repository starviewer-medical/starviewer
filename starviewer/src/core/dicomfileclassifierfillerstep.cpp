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

#include <QApplication> //Per el process events, TODO Treure i fer amb threads.

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
            if (classifyFile(file))
            {
                m_input->addLabel("DICOMFileClassifierFillerStep");
            }
            qApp->processEvents();
        }
    }
    else
    {
        DEBUG_LOG("No tenim input!");
    }

    return ok;
}

bool DICOMFileClassifierFillerStep::fillIndividually()
{
    Q_ASSERT(m_input);

    m_dicomReader = m_input->getDICOMFile();
    classifyFile();

    m_input->addLabel("DICOMFileClassifierFillerStep");

    return true;
}

bool DICOMFileClassifierFillerStep::classifyFile( QString file )
{
    // no està classificat per tal l'hem de tractar
    bool ok = m_dicomReader->setFile( file );
    if( ok )
    {
        classifyFile();
    }
    else
    {
        DEBUG_LOG("Error en llegir l'arxiu: " + file );
    }

    return ok;
}

void DICOMFileClassifierFillerStep::classifyFile()
{
    Q_ASSERT(m_dicomReader);

    // primer recopilem tota la informació que ens permet ubicar l'arxiu dins de l'estructura
    QString patientID = m_dicomReader->getAttributeByName( DCM_PatientID );
    QString studyUID = m_dicomReader->getAttributeByName( DCM_StudyInstanceUID );
    QString seriesUID = m_dicomReader->getAttributeByName( DCM_SeriesInstanceUID );

    // fem una classificació top-down. Comencem mirant a quin pacient pertany,després estudi, serie fins arribar al nivell
    // d'imatge/kin/PS. TODO potser seria més eficient començar directament per imatge? En cas de descartar aniríem més
    // ràpid o no? o és ben igual?
    // obtenim el pacient si ja existeix, altrament el creem
    Patient *patient = m_input->getPatientByID( patientID );
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
    series->addFilePath( m_dicomReader->getFileName() );

    m_input->setCurrentSeries(series);
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
    patient->setBirthDate( m_dicomReader->getAttributeByName( DCM_PatientsBirthDate ) );
    patient->setSex( m_dicomReader->getAttributeByName( DCM_PatientsSex ) );

    return patient;
}

Study *DICOMFileClassifierFillerStep::createStudy()
{
    Study *study = new Study;

    study->setInstanceUID( m_dicomReader->getAttributeByName( DCM_StudyInstanceUID ) );
    study->setDate( m_dicomReader->getAttributeByName( DCM_StudyDate ) );
    study->setTime( m_dicomReader->getAttributeByName( DCM_StudyTime ) );
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

    series->setInstanceUID( m_dicomReader->getAttributeByName( DCM_SeriesInstanceUID ) );
    series->setModality( m_dicomReader->getAttributeByName( DCM_Modality ) );
    series->setSeriesNumber( m_dicomReader->getAttributeByName( DCM_SeriesNumber ) );
    series->setDate( m_dicomReader->getAttributeByName( DCM_SeriesDate ) );
    series->setTime( m_dicomReader->getAttributeByName( DCM_SeriesTime ) );
    series->setInstitutionName( m_dicomReader->getAttributeByName( DCM_InstitutionName ) );
    series->setPatientPosition( m_dicomReader->getAttributeByName( DCM_PatientPosition ) );
    series->setProtocolName( m_dicomReader->getAttributeByName( DCM_ProtocolName ) );
    series->setDescription( m_dicomReader->getAttributeByName( DCM_SeriesDescription ) );
    series->setFrameOfReferenceUID( m_dicomReader->getAttributeByName( DCM_FrameOfReferenceUID ) );
    series->setPositionReferenceIndicator( m_dicomReader->getAttributeByName( DCM_PositionReferenceIndicator ) );
    series->setManufacturer( m_dicomReader->getAttributeByName( DCM_Manufacturer ) );

    return series;
}

}
