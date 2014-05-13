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

#include "status.h"

// Provide the OFcondition structure and his members
#include <ofcond.h>

namespace udg {

Status::Status()
{
    m_numberError = -1;
}

bool Status::good() const
{
    return m_success;
}

QString Status::text() const
{
    return m_descText;
}

int Status::code() const
{
    return m_numberError;
}

Status Status::setStatus(const OFCondition status)
{
    m_descText = status.text();
    m_success = status.good();
    m_numberError = status.code();

    return *this;
}

Status Status::setStatus(QString desc, bool ok, int numError)
{
    m_descText = desc;
    m_success = ok;
    m_numberError = numError;

    return *this;
}

};
