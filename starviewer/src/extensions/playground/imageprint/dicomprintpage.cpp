#include "dicomprintpage.h"

#include <QRegExp>
#include <QStringList>

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

int DicomPrintPage::getFilmLayoutColumns()
{
    int columns, rows;

    getFilmLayoutColumnsRows(columns, rows);

    return columns;
}

int DicomPrintPage::getFilmLayoutRows()
{
    int columns, rows;

    getFilmLayoutColumnsRows(columns, rows);

    return rows;
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

void DicomPrintPage::getFilmLayoutColumnsRows(int &columns, int &rows)
{
    QStringList splittedFilmLayout;
    QString filmLayout = getFilmLayout();
    /*Comprovem amb expresions regulars que sigui un film layout correcte tenir en compte que en c++ per indicar '\' hem d'escriure '\\'
      en el cas com és aquest que l'expresiò regular conté un '\', en expresió regular s'escriu '\\', i en codi ho hem d'escriur com '\\\\' */

    if (filmLayout.contains(QRegExp("STANDARD\\\\\\d,\\d", Qt::CaseInsensitive))) //STANDARD\C,R
    {
        filmLayout.remove("STANDARD\\");
        splittedFilmLayout = filmLayout.split(",");

        columns = splittedFilmLayout.at(0).toInt();
        rows = splittedFilmLayout.at(1).toInt();
    }
    else
    {
        columns = -1;
        rows = -1;
    }
    //TODO: falten per implementar obtenir el número de columnes i files pels altres Layouts que també contempla el DICOM (PS 3.3 pàg 918 Tag Image Display Format)
}

}