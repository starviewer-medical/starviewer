
#include "printer.h"
#include "dicomprinter.h"
#include "dcmtk/dcmdata/dctypes.h"    /* for Uint32 */
#include <QString>

namespace udg{

DicomPrinter::DicomPrinter()
{	
}

 
DicomPrinter::~DicomPrinter()
{
}

void DicomPrinter::setName(const QString &_name)
{
    m_name=_name;
}

QString DicomPrinter::getName() const
{
    return m_name;
}

void DicomPrinter::setType(const QString &_type)
{
    m_type=_type;
}

QString DicomPrinter::getType() const
{
	return m_type;
}

void DicomPrinter::setDescription(const QString &_description)
{
    m_description=_description;
}

QString DicomPrinter::getDescription() const
{
    return m_description;
}

void DicomPrinter::setHostname(const QString &_hostname)
{
    m_hostname=_hostname;
}

QString DicomPrinter::getHostname() const
{
    return m_hostname;
}

void DicomPrinter::setPort(const int &_port)
{
    m_port=_port;
}

int DicomPrinter::getPort() const
{
    return m_port;
}

void DicomPrinter::setMediumType(const QString &_mediumType)
{
    m_mediumType=_mediumType;
}


QString DicomPrinter::getMediumType() const
{
    return m_mediumType;
}

void DicomPrinter::setFilmSize(const QString &filmSize)
{
    m_filmSize=filmSize;
}

QString DicomPrinter::getFilmSize() const
{
    return m_filmSize;
}

void DicomPrinter::setFilmLayout(const QString &_filmLayout)
{
    m_filmLayout=_filmLayout;
}

QString DicomPrinter::getFilmLayout() const
{
    return m_filmLayout;
}

void  DicomPrinter::setMagnificationType(const QString &_magnificationType)
{
    m_magnificationType=_magnificationType;
}

QString DicomPrinter::getMagnificationType() const
{
    return m_magnificationType;
}

void DicomPrinter::setMaxDensity(ushort maxDensity)
{
    m_maxDensity = maxDensity;
}

ushort DicomPrinter::getMaxDensity()
{
    return m_maxDensity;
}

void DicomPrinter::setMinDensity(ushort minDensity)
{
    m_minDensity = minDensity;
}

ushort DicomPrinter::getMinDensity()
{
    return m_minDensity;
}

void DicomPrinter::setTrim(bool _trim)
{
    m_trim=_trim;
}

bool DicomPrinter::getTrim() const
{
    return m_trim;
}

void  DicomPrinter::setBorderDensity(const QString &_borderDensity)
{
    m_borderDensity=_borderDensity;
}

QString DicomPrinter::getBorderDensity() const
{
    return m_borderDensity;
}

void  DicomPrinter::setEmptyImageDensity(const QString &_emptyImageDensity)
{
    m_emptyImageDensity=_emptyImageDensity;
}

QString DicomPrinter::getEmptyImageDensity() const
{
    return m_emptyImageDensity;
}

void  DicomPrinter::setFilmDestination(const QString &_filmDestination)
{
    m_filmDestination=_filmDestination;
}

QString DicomPrinter::getFilmDestination() const
{
    return m_filmDestination;
}

void DicomPrinter::setSmoothingType(const QString &smoothingType)
{
    m_smoothingType = smoothingType;
}

QString DicomPrinter::getSmoothingType() const
{
    return m_smoothingType;
}

void DicomPrinter::setJobPriority(const QString &priority)
{
    m_priority = priority;
}

QString DicomPrinter::getJobPriority() const
{
    return m_priority;
}

void DicomPrinter::setPolarity(const QString &polarity)
{
    m_polarity = polarity;
}

QString DicomPrinter::getPolarity() const
{
    return m_polarity;
}

DicomPrinter& DicomPrinter::operator= (const DicomPrinter& _printer)
{
    m_name                  =_printer.getName();
    m_hostname              =_printer.getHostname();
    m_port                  =_printer.getPort();
    m_description           =_printer.getDescription();
    m_mediumType            =_printer.getMediumType();
    m_filmSize              =_printer.getFilmSize();
    m_filmLayout            =_printer.getFilmLayout();
    m_magnificationType     =_printer.getMagnificationType();
    m_trim                  =_printer.getTrim();
    m_borderDensity         =_printer.getBorderDensity();
    m_emptyImageDensity     =_printer.getEmptyImageDensity();
    m_filmDestination       =_printer.getFilmDestination();
	return *this;
}
}