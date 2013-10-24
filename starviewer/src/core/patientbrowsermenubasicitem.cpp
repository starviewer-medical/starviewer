#include "patientbrowsermenubasicitem.h"

namespace udg {

PatientBrowserMenuBasicItem::PatientBrowserMenuBasicItem(QObject *parent)
: QObject(parent)
{
}

void PatientBrowserMenuBasicItem::setText(const QString &text)
{
    m_text = text;
    emit textChanged();
}

QString PatientBrowserMenuBasicItem::getText()
{
    return m_text;
}

void PatientBrowserMenuBasicItem::setIdentifier(const QString &identifier)
{
    m_identifier = identifier;
    emit identifierChanged();
}

QString PatientBrowserMenuBasicItem::getIdentifier() const
{
    return m_identifier;
}

QString PatientBrowserMenuBasicItem::getType()
{
    return "BasicItem";
}

}
