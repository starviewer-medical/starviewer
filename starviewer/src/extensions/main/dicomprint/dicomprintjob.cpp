#include <QString>

#include "dicomprintjob.h"

namespace udg {

void DicomPrintJob::setPrintPriority(const QString& printPriority)
{
    m_printPriority = printPriority;
}

QString DicomPrintJob::getPrintPriority() const
{
    return m_printPriority;
}

void DicomPrintJob::setMediumType(const QString& mediumType)
{
    m_mediumType = mediumType;
}

QString DicomPrintJob::getMediumType() const
{
    return m_mediumType;
}

void DicomPrintJob::setLabel(const QString& label)
{
    m_label = label;
}

QString DicomPrintJob::getLabel() const
{
    return m_label;
}

void DicomPrintJob::setFilmDestination(const QString& filmDestination)
{
    m_filmDestination = filmDestination;
}

QString DicomPrintJob::getFilmDestination() const
{
    return m_filmDestination;
}

void DicomPrintJob::setDicomPrintPages(QList<DicomPrintPage> dicomPrintPages)
{
    m_listDicomPrintPage = dicomPrintPages;
}

QList<DicomPrintPage> DicomPrintJob::getDicomPrintPages()
{
    return m_listDicomPrintPage;
}

void DicomPrintJob::setNumberOfCopies(int numberOfCopies)
{
    m_numberOfCopies = numberOfCopies;
}

int DicomPrintJob::getNumberOfCopies()
{
    return m_numberOfCopies;
}

}
