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
