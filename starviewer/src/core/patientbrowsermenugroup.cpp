#include "patientbrowsermenugroup.h"

namespace udg {

PatientBrowserMenuGroup::PatientBrowserMenuGroup(QObject *parent) :
    QObject(parent)
{
}

void PatientBrowserMenuGroup::setCaption(const QString &caption)
{
    m_caption = caption;
}

QString PatientBrowserMenuGroup::getCaption() const
{
    return m_caption;
}

void PatientBrowserMenuGroup::setElements(const QList<PatientBrowserMenuBasicItem*> &elements)
{
    m_elements = elements;
}

QList<PatientBrowserMenuBasicItem*> PatientBrowserMenuGroup::getElements() const
{
    return m_elements;
}

QList<QObject*> PatientBrowserMenuGroup::getElementsAsQObject() const
{
    QList<QObject*> output;
    foreach (PatientBrowserMenuBasicItem* item, m_elements)
    {
        output << item;
    }

    return output;
}

void PatientBrowserMenuGroup::setFusionElements(const QList<PatientBrowserMenuBasicItem*> &elements)
{
    m_fusionElements = elements;
}

QList<PatientBrowserMenuBasicItem*> PatientBrowserMenuGroup::getFusionElements() const
{
    return m_elements;
}

QList<QObject*> PatientBrowserMenuGroup::getFusionElementsAsQObject() const
{
    QList<QObject*> output;
    foreach (PatientBrowserMenuBasicItem* item, m_fusionElements)
    {
        output << item;
    }

    return output;
}

} // namespace udg
