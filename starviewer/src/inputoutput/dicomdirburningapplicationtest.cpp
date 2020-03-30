/*************************************************************************************
  Copyright (C) 2014 Laboratori de Gràfics i Imatge, Universitat de Girona &
  Institut de Diagnòstic per la Imatge.
  Girona 2014. All rights reserved.
  http://starviewer.udg.edu

  This file is part of the Starviewer (Medical Imaging Software) open source project.
  It is subject to the license terms in the LICENSE file found in the top-level
  directory of this distribution and at http://starviewer.udg.edu/license. No part of
  the Starviewer (Medical Imaging Software) open source project, including this file,
  may be copied, modified, propagated, or distributed except according to the
  terms contained in the LICENSE file.
 *************************************************************************************/

#include "dicomdirburningapplicationtest.h"
#include "inputoutputsettings.h"
#include "starviewerapplication.h"

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
        problem.setSolution(tr("Install a DICOMDIR burning application and configure %1 to use it on Tools > Configuration > DICOMDIR").arg(ApplicationNameString));
        result.addWarning(problem);
    }

    return result;
}

QString DICOMDIRBurningApplicationTest::getDescription() const
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
