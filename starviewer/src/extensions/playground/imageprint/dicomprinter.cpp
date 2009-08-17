
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

void DicomPrinter::setFileSize(const QString &_fileSize)
{
    m_fileSize=_fileSize;
}

QString DicomPrinter::getFileSize() const
{
    return m_fileSize;
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

void DicomPrinter::setDefaultReflection(const int &_defaultReflection)
{
    m_defaultReflection=_defaultReflection;
}

int DicomPrinter::getDefaultReflection() const
{
    return m_defaultReflection;
}

void DicomPrinter::setMaxPrintResolution(const int &_x, const int &_v)
{
    m_maxPrintResolutionH=_x;
    m_maxPrintResolutionV=_v;
}

int DicomPrinter::getMaxPrintResolutionH() const
{
    return m_maxPrintResolutionH;
}

int DicomPrinter::getMaxPrintResolutionV()const
{
    return m_maxPrintResolutionV;
}


void DicomPrinter::setMinPrintResolution(const int &_x, const int &_v)
{
    m_minPrintResolutionH=_x;
    m_minPrintResolutionV=_v;
}

int DicomPrinter::getMinPrintResolutionH()const
{
    return m_minPrintResolutionH;
}

int DicomPrinter::getMinPrintResolutionV()const
{
    return m_minPrintResolutionV;
}

void DicomPrinter::setMaxPreviewResolution(const int &_x, const int &_v)
{
    m_maxPreviewResolutionH=_x;
    m_maxPreviewResolutionV=_v;
}

int DicomPrinter::getMaxPreviewResolutionH() const
{
    return m_maxPreviewResolutionH;
}

int DicomPrinter::getMaxPreviewResolutionV() const
{
    return m_maxPreviewResolutionV;
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
    m_fileSize              =_printer.getFileSize();
    m_filmLayout            =_printer.getFilmLayout();
    m_magnificationType     =_printer.getMagnificationType();
    m_defaultReflection     =_printer.getDefaultReflection();
    m_maxPrintResolutionH   =_printer.getMaxPrintResolutionH();
    m_maxPrintResolutionV   =_printer.getMaxPrintResolutionV();
    m_minPrintResolutionH   =_printer.getMinPrintResolutionH();
    m_minPrintResolutionV   =_printer.getMinPrintResolutionV();
    m_maxPreviewResolutionH =_printer.getMaxPreviewResolutionH();
    m_maxPreviewResolutionV =_printer.getMaxPreviewResolutionV();
    m_trim                  =_printer.getTrim();
    m_borderDensity         =_printer.getBorderDensity();
    m_emptyImageDensity     =_printer.getEmptyImageDensity();
    m_filmDestination       =_printer.getFilmDestination();
	return *this;
}
}