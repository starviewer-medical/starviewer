#include "leanbodymassformulacalculator.h"

#include "image.h"
#include "series.h"
#include "study.h"
#include "patient.h"
#include "dicomtagreader.h"
#include "logging.h"

namespace udg {

LeanBodyMassFormulaCalculator::LeanBodyMassFormulaCalculator()
{
    initializeParameters();
}

LeanBodyMassFormulaCalculator::~LeanBodyMassFormulaCalculator()
{
}

void LeanBodyMassFormulaCalculator::initializeParameters()
{
    m_patientsWeightInKg = 0;
    m_patientsHeightInCm = 0;
    m_patientsSex = "";
}

bool LeanBodyMassFormulaCalculator::parameterValuesAreValid() const
{
    if (m_patientsWeightInKg > 0 && m_patientsHeightInCm > 0 && (m_patientsSex == "M" || m_patientsSex == "F"))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool LeanBodyMassFormulaCalculator::canCompute()
{
    gatherRequiredParameters();
    return parameterValuesAreValid();
}

double LeanBodyMassFormulaCalculator::compute()
{
    gatherRequiredParameters();
    
    if (m_patientsSex == "M")
    {
        return computeMale(m_patientsHeightInCm, m_patientsWeightInKg);
    }
    else if (m_patientsSex == "F")
    {
        return computeFemale(m_patientsHeightInCm, m_patientsWeightInKg);
    }
    else
    {
        DEBUG_LOG("Patient's sex value is neither M nor F. Cannot compute Lean Body Mass");
        return 0.0;
    }
}

void LeanBodyMassFormulaCalculator::gatherRequiredParameters()
{
    initializeParameters();
    if (m_imageSource)
    {
        gatherRequiredParameters(m_imageSource);
    }
    else if (m_tagReaderSource)
    {
        gatherRequiredParameters(m_tagReaderSource);
    }
}

void LeanBodyMassFormulaCalculator::gatherRequiredParameters(Image *image)
{
    if (!image)
    {
        return;
    }
    
    Series *imageSeries = image->getParentSeries();
    if (imageSeries)
    {
        Study *imageStudy = imageSeries->getParentStudy();
        if (imageStudy)
        {
            m_patientsWeightInKg = imageStudy->getWeight();
            m_patientsHeightInCm = imageStudy->getHeight() * 100;

            Patient *imagePatient = imageStudy->getParentPatient();
            if (imagePatient)
            {
                m_patientsSex =  imagePatient->getSex();
            }
            else
            {
                DEBUG_LOG("Lean Body Mass cannot be computed with provided Image. Patient's sex could not be retrieved, image's Patient is null.");
            }
        }
        else
        {
            DEBUG_LOG("Lean Body Mass cannot be computed with provided Image. Patient's height and weight could not be retrieved, image's Study is null.");
        }
    }
    else
    {
        DEBUG_LOG("Lean Body Mass cannot be computed with provided Image. Patient's height and weight could not be retrieved, image's Series is null.");
    }
}

void LeanBodyMassFormulaCalculator::gatherRequiredParameters(DICOMTagReader *tagReader)
{
    if (!tagReader)
    {
        return;
    }

    m_patientsWeightInKg = tagReader->getValueAttributeAsQString(DICOMPatientWeight).toInt();
    m_patientsHeightInCm = tagReader->getValueAttributeAsQString(DICOMPatientSize).toDouble() * 100;
    m_patientsSex = tagReader->getValueAttributeAsQString(DICOMPatientSex);
}

} // End namespace udg
