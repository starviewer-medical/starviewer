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

#include "echotowadopacs.h"

#include "dicommask.h"
#include "studyoperationresult.h"
#include "studyoperationsservice.h"

namespace udg {

EchoToWadoPacs::EchoToWadoPacs()
{
}

bool EchoToWadoPacs::echo(const PacsDevice &pacs)
{
    DicomMask mask;
    mask.setStudyDate(QDate::currentDate().addYears(1), QDate::currentDate().addYears(1));
    StudyOperationResult *result = StudyOperationsService::instance()->searchPacs(pacs, mask, StudyOperations::TargetResource::Studies);
    m_errors = result->getErrorText();
    bool success = result->getResultType() != StudyOperationResult::ResultType::Error;
    delete result;
    return success;
}

const QString& EchoToWadoPacs::getErrors() const
{
    return m_errors;
}

} // namespace udg
