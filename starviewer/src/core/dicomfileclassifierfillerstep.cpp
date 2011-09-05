#include "dicomfileclassifierfillerstep.h"
#include "logging.h"
#include "dicomtagreader.h"
#include "patientfillerinput.h"
#include "patient.h"
#include "study.h"
#include "series.h"
#include "image.h"
#include "createinformationmodelobject.h"

namespace udg {

DICOMFileClassifierFillerStep::DICOMFileClassifierFillerStep()
 : PatientFillerStep()
{
    m_dicomReader = new DICOMTagReader;
}

DICOMFileClassifierFillerStep::~DICOMFileClassifierFillerStep()
{
}

bool DICOMFileClassifierFillerStep::fillIndividually()
{
    Q_ASSERT(m_input);

    m_dicomReader = m_input->getDICOMFile();
    classifyFile();

    m_input->addLabel("DICOMFileClassifierFillerStep");

    return true;
}

void DICOMFileClassifierFillerStep::classifyFile()
{
    Q_ASSERT(m_dicomReader);

    // Pprimer recopilem tota la informació que ens permet ubicar l'arxiu dins de l'estructura
    QString patientID = m_dicomReader->getValueAttributeAsQString(DICOMPatientID);
    QString studyUID = m_dicomReader->getValueAttributeAsQString(DICOMStudyInstanceUID);
    QString seriesUID = m_dicomReader->getValueAttributeAsQString(DICOMSeriesInstanceUID);

    // Fem una classificació top-down. Comencem mirant a quin pacient pertany,després estudi, serie fins arribar al nivell
    // d'imatge/kin/PS. TODO potser seria més eficient començar directament per imatge? En cas de descartar aniríem més
    // ràpid o no? o és ben igual?
    // Obtenim el pacient si ja existeix, altrament el creem
    Patient *patient = m_input->getPatientByID(patientID);
    if (!patient)
    {
        patient = CreateInformationModelObject::createPatient(m_dicomReader);
        m_input->addPatient(patient);
    }

    // Obtenim l'estudi corresponent si ja existeix, altrament el creem
    Study *study = patient->getStudy(studyUID);
    if (!study)
    {
        study = CreateInformationModelObject::createStudy(m_dicomReader);
        patient->addStudy(study);
    }

    // Obtenim la serie corresponent si ja existeix, altrament la creem
    Series *series = study->getSeries(seriesUID);
    if (!series)
    {
        series = CreateInformationModelObject::createSeries(m_dicomReader);
        study->addSeries(series);
        // Per cada sèrie reiniciem el número de volum multiframe i single frame
        m_input->resetCurrentMultiframeVolumeNumber();
        m_input->resetCurrentSingleFrameVolumeNumber();
    }

    m_input->setCurrentSeries(series);
}

}
