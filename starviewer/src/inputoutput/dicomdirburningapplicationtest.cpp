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
    DiagnosisTestResult result;

    Settings settings;
    QString burningApplicationExecutable = settings.getValue(InputOutputSettings::DICOMDIRBurningApplicationPathKey).toString();
    
    if (burningApplicationIsDefined(burningApplicationExecutable))
    {
        if (!burningApplicationIsInstalled(burningApplicationExecutable))
        {
            DiagnosisTestProblem problem;
            problem.setState(DiagnosisTestProblem::Error);
            problem.setDescription(tr("DICOMDIR burning application is not installed on the given location"));
            problem.setSolution(tr("Provide the correct location on Tools > Configuration > DICOMDIR"));
            result.addError(problem);
        }
    }
    else
    {
        DiagnosisTestProblem problem;
        problem.setState(DiagnosisTestProblem::Warning);
        problem.setDescription(tr("There is no DICOMDIR burning application configured"));
        problem.setSolution(tr("Install a DICOMDIR burning application and configure Starviewer to use it on Tools > Configuration > DICOMDIR"));
        result.addWarning(problem);
    }

    return result;
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
