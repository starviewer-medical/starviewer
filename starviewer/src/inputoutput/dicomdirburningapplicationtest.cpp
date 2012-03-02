#include "dicomdirburningapplicationtest.h"
#include "inputoutputsettings.h"

#include <QFile>
#include <QString>

namespace udg {

DICOMDIRBurningApplicationTest::DICOMDIRBurningApplicationTest(QObject *parent)
 : DiagnosisTest(parent)
{
}

DICOMDIRBurningApplicationTest::~DICOMDIRBurningApplicationTest()
{
}

DiagnosisTestResult DICOMDIRBurningApplicationTest::run()
{
    DiagnosisTestResult::DiagnosisTestResultState testResultState = DiagnosisTestResult::Invalid;
    QString testResultDescription;
    QString testResultSolution;

    Settings settings;
    QString burningApplicationExecutable = settings.getValue(InputOutputSettings::DICOMDIRBurningApplicationPathKey).toString();
    
    if (burningApplicationIsDefined(burningApplicationExecutable))
    {
        if (burningApplicationIsInstalled(burningApplicationExecutable))
        {
            testResultState = DiagnosisTestResult::Ok;
        }
        else
        {
            testResultState = DiagnosisTestResult::Error;
            testResultDescription = tr("DICOMDIR burning application is not installed on the given location.");
            testResultSolution = tr("Provide the correct location on Tools > Configuration > DICOMDIR.");
        }
    }
    else
    {
        testResultState = DiagnosisTestResult::Warning;
        testResultDescription = tr("There is no DICOMDIR burning application configured.");
        testResultSolution = tr("Install a DICOMDIR burning application and configure Starviewer to use it on Tools > Configuration > DICOMDIR.");
    }

    return DiagnosisTestResult(testResultState, testResultDescription, testResultSolution);
}

QString DICOMDIRBurningApplicationTest::getDescription()
{
    return tr("The application to burn DICOMDIRs to CD/DVD is correctly configured");
}

bool DICOMDIRBurningApplicationTest::burningApplicationIsDefined(const QString &burningApplication)
{
    return !burningApplication.isEmpty();
}

bool DICOMDIRBurningApplicationTest::burningApplicationIsInstalled(const QString &file)
{
    return QFile::exists(file);
}

} // end namespace udg
