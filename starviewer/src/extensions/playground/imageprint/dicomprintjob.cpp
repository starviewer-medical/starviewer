#include <QString>
#include <QSettings>
#include "dicomprintjob.h"

namespace udg 
{	
DicomPrintJob::DicomPrintJob()
{	
}

 
DicomPrintJob::~DicomPrintJob()
{
}

void DicomPrintJob::setNumberCopies(const int &_numberCopies)
{	
    m_numberCopies=_numberCopies;
}

int DicomPrintJob::getNumberCopies() const
{
   return m_numberCopies;
}

void DicomPrintJob::setPriority(const QString &_priority)
{
    m_priority=_priority;
}

QString DicomPrintJob::getPriority() const
{
    return m_priority;
}

void DicomPrintJob::setMediumType(const int &_mediumType)
{
    m_mediumType=_mediumType;
}

int DicomPrintJob::getMediumType() const
{
    return m_mediumType;
}

void DicomPrintJob::setLabel(const QString &_label)
{
    m_label=_label;
}

QString DicomPrintJob::getLabel() const
{
    return m_label;
}

void DicomPrintJob::setFilmDestination(const QString &_filmDestination)
{
    m_filmDestination=_filmDestination;
}

QString DicomPrintJob::getFilmDestination() const
{
    return m_filmDestination;
}
}