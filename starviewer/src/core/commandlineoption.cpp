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
