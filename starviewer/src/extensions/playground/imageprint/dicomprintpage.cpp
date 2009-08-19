#include "dicomprintpage.h"

namespace udg 
{

void DicomPrintPage::setFilmSize(const QString &filmSize)
{
    m_filmSize = filmSize;
}

QString DicomPrintPage::getFilmSize() const
{
    return m_filmSize;
}

void DicomPrintPage::setFilmLayout(const QString &filmLayout)
{
    m_filmLayout = filmLayout;
}

QString DicomPrintPage::getFilmLayout() const
{
    return m_filmLayout;
}

void DicomPrintPage::setFilmOrientation(const QString &filmOrientation)
{
    m_filmOrientation =  filmOrientation;
}

QString DicomPrintPage::getFilmOrientation() const
{
    return m_filmOrientation;
}

void DicomPrintPage::setMagnificationType(const QString &magnificationType)
{
    m_magnificationType = magnificationType;
}

QString DicomPrintPage::getMagnificationType() const
{
    return m_magnificationType;
}

void DicomPrintPage::setMaxDensity(ushort maxDensity)
{
    m_maxDensity = maxDensity;
}

ushort DicomPrintPage::getMaxDensity()
{
    return m_maxDensity;
}

void DicomPrintPage::setMinDensity(ushort minDensity)
{
    m_minDensity = minDensity;
}

ushort DicomPrintPage::getMinDensity()
{
    return m_minDensity;
}

void DicomPrintPage::setTrim(bool trim)
{
    m_trim = trim;
}

bool DicomPrintPage::getTrim()
{
    return m_trim;
}

void DicomPrintPage::setBorderDensity(const QString &borderDensity)
{
    m_borderDensity = borderDensity;
}

QString DicomPrintPage::getBorderDensity() const
{
    return m_borderDensity;
}

void DicomPrintPage::setEmptyImageDensity(const QString &emptyImageDensity)
{
    m_emptyImageDensity = emptyImageDensity;
}

QString DicomPrintPage::getEmptyImageDensity() const
{
    return m_emptyImageDensity;
}

void DicomPrintPage::setSmoothingType(const QString &smoothingType)
{
    m_smoothingType = smoothingType;
}

QString DicomPrintPage::getSmoothingType() const
{
    return m_smoothingType;
}

void DicomPrintPage::setPolarity(const QString &polarity)
{
    m_polarity = polarity;
}

QString DicomPrintPage::getPolarity() const
{
    return m_polarity;
}
}