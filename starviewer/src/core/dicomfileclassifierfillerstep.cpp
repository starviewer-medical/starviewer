/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "dicomfileclassifierfillerstep.h"
#include "logging.h"
#include "dicomtagreader.h"
#include "dicomdictionary.h"
#include "patientfillerinput.h"
#include "patient.h"
#include "study.h"
#include "series.h"
#include "image.h"
#include "createinformationmodelobject.h"

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
    QString patientID = m_dicomReader->getValueAttributeAsQString( DICOMPatientID );
    QString studyUID = m_dicomReader->getValueAttributeAsQString( DICOMStudyInstanceUID );
    QString seriesUID = m_dicomReader->getValueAttributeAsQString( DICOMSeriesInstanceUID );

    // fem una classificació top-down. Comencem mirant a quin pacient pertany,després estudi, serie fins arribar al nivell
    // d'imatge/kin/PS. TODO potser seria més eficient començar directament per imatge? En cas de descartar aniríem més
    // ràpid o no? o és ben igual?
    // obtenim el pacient si ja existeix, altrament el creem
    Patient *patient = m_input->getPatientByID( patientID );
    if( !patient )
    {
        patient = CreateInformationModelObject::createPatient(m_dicomReader);
        m_input->addPatient( patient );
    }

    // obtenim l'estudi corresponent si ja existeix, altrament el creem
    Study *study = patient->getStudy( studyUID );
    if( !study )
    {
        study = CreateInformationModelObject::createStudy(m_dicomReader);
        patient->addStudy( study );
    }

    // obtenim la serie corresponent si ja existeix, altrament la creem
    Series *series = study->getSeries( seriesUID );
    if( !series )
    {
        series = CreateInformationModelObject::createSeries(m_dicomReader);
        study->addSeries( series );
        // Per cada sèrie reiniciem el número de volum multiframe
        m_input->resetCurrentMultiframeVolumeNumber();
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

}
