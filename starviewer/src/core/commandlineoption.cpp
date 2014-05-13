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

#include "commandlineoption.h"

#include <QObject>

namespace udg {

const QString CommandLineOption::OptionSelectorPrefix("-");

CommandLineOption::CommandLineOption()
{
    m_argumentIsRequired = false;
}

CommandLineOption::CommandLineOption(const QString &name, bool argumentIsRequired, const QString &description)
{
    m_name = name;
    m_argumentIsRequired = argumentIsRequired;
    m_description = description;
}

CommandLineOption::~CommandLineOption()
{
}

QString CommandLineOption::getName() const
{
    return m_name;
}
        
QString CommandLineOption::getDescription() const
{
    return m_description;
}

bool CommandLineOption::requiresArgument() const
{
    return m_argumentIsRequired;
}

QString CommandLineOption::toString(bool includeDescription) const
{
    QString option = OptionSelectorPrefix + m_name;

    if (m_argumentIsRequired)
    {
        option += " " + QObject::tr("value");
    }

    if (includeDescription)
    {
        option += " \t" + m_description;
    }

    return option;
}

}  // End namespace udg
