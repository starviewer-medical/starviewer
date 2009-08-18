#include "dicomprinter.h"

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

void DicomPrinter::setDefaultMediumType(const QString &_defaultMediumType)
{
    m_defaultMediumType=_defaultMediumType;
}


QString DicomPrinter::getDefaultMediumType() const
{
    return m_defaultMediumType;
}

void DicomPrinter::setDefaultFilmSize(const QString &defaultFilmSize)
{
    m_defaultFilmSize = defaultFilmSize;
}

QString DicomPrinter::getDefaultFilmSize() const
{
    return m_defaultFilmSize;
}

void DicomPrinter::setDefaultFilmLayout(const QString &_defaultFilmLayout)
{
    m_defaultFilmLayout=_defaultFilmLayout;
}

QString DicomPrinter::getDefaultFilmLayout() const
{
    return m_defaultFilmLayout;
}

void  DicomPrinter::setDefaultMagnificationType(const QString &_defaultMagnificationType)
{
    m_defaultMagnificationType=_defaultMagnificationType;
}

QString DicomPrinter::getDefaultMagnificationType() const
{
    return m_defaultMagnificationType;
}

void DicomPrinter::setDefaultMaxDensity(ushort defaultMaxDensity)
{
    m_defaultMaxDensity = defaultMaxDensity;
}

ushort DicomPrinter::getDefaultMaxDensity()
{
    return m_defaultMaxDensity;
}

void DicomPrinter::setDefaultMinDensity(ushort defaultMinDensity)
{
    m_defaultMinDensity = defaultMinDensity;
}

ushort DicomPrinter::getDefaultMinDensity()
{
    return m_defaultMinDensity;
}

void DicomPrinter::setDefaultTrim(bool _defaultTrim)
{
    m_defaultTrim=_defaultTrim;
}

bool DicomPrinter::getDefaultTrim() const
{
    return m_defaultTrim;
}

void  DicomPrinter::setDefaultBorderDensity(const QString &_defaultBorderDensity)
{
    m_defaultBorderDensity=_defaultBorderDensity;
}

QString DicomPrinter::getDefaultBorderDensity() const
{
    return m_defaultBorderDensity;
}

void  DicomPrinter::setDefaultEmptyImageDensity(const QString &_defaultEmptyImageDensity)
{
    m_defaultEmptyImageDensity=_defaultEmptyImageDensity;
}

QString DicomPrinter::getDefaultEmptyImageDensity() const
{
    return m_defaultEmptyImageDensity;
}

void  DicomPrinter::setDefaultFilmDestination(const QString &_defaultFilmDestination)
{
    m_defaultFilmDestination=_defaultFilmDestination;
}

QString DicomPrinter::getDefaultFilmDestination() const
{
    return m_defaultFilmDestination;
}

void DicomPrinter::setDefaultSmoothingType(const QString &defaultSmoothingType)
{
    m_defaultSmoothingType = defaultSmoothingType;
}

QString DicomPrinter::getDefaultSmoothingType() const
{
    return m_defaultSmoothingType;
}

void DicomPrinter::setDefaultJobPriority(const QString &defaultPriority)
{
    m_defaultPriority = defaultPriority;
}

QString DicomPrinter::getDefaultJobPriority() const
{
    return m_defaultPriority;
}

void DicomPrinter::setDefaultPolarity(const QString &defaultPolarity)
{
    m_defaultPolarity = defaultPolarity;
}

QString DicomPrinter::getDefaultPolarity() const
{
    return m_defaultPolarity;
}

DicomPrinter& DicomPrinter::operator= (const DicomPrinter& _printer)
{
    m_name                  =_printer.getName();
    m_hostname              =_printer.getHostname();
    m_port                  =_printer.getPort();
    m_description           =_printer.getDescription();
    m_defaultMediumType            =_printer.getDefaultMediumType();
    m_defaultFilmSize              =_printer.getDefaultFilmSize();
    m_defaultFilmLayout            =_printer.getDefaultFilmLayout();
    m_defaultMagnificationType     =_printer.getDefaultMagnificationType();
    m_defaultTrim                  =_printer.getDefaultTrim();
    m_defaultBorderDensity         =_printer.getDefaultBorderDensity();
    m_defaultEmptyImageDensity     =_printer.getDefaultEmptyImageDensity();
    m_defaultFilmDestination       =_printer.getDefaultFilmDestination();
	return *this;
}
}