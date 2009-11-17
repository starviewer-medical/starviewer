#include "dicomprinter.h"

namespace udg{

DicomPrinter::DicomPrinter()
{
    m_ID = -1;
    m_defaultMaxDensity = 0;
    m_maxDensityValues = 0;
    m_defaultMinDensity = 0;
    m_minDensityValues = 0;
}

 
DicomPrinter::~DicomPrinter()
{
}

void DicomPrinter::setID(int ID)
{
    m_ID = ID;
}

int DicomPrinter::getID()
{
    return m_ID;
}

void DicomPrinter::setName(const QString &name)
{
    m_name = name;
}

QString DicomPrinter::getName() const
{
    return m_name;
}

void DicomPrinter::setType(const QString &type)
{
    m_type = type;
}

QString DicomPrinter::getType() const
{
	return m_type;
}

void DicomPrinter::setDescription(const QString &description)
{
    m_description = description;
}

QString DicomPrinter::getDescription() const
{
    return m_description;
}

void DicomPrinter::setAETitle(const QString &aeTitle)
{
    m_aeTitle = aeTitle;
}

QString DicomPrinter::getAETitle() const
{
    return m_aeTitle;
}

void DicomPrinter::setHostname(const QString &hostname)
{
    m_hostname = hostname;
}

QString DicomPrinter::getHostname() const
{
    return m_hostname;
}

void DicomPrinter::setPort(const int &port)
{
    m_port = port;
}

int DicomPrinter::getPort() const
{
    return m_port;
}

void DicomPrinter::setDefaultMediumType(const QString &defaultMediumType)
{
    m_defaultMediumType = defaultMediumType;
}

QString DicomPrinter::getDefaultMediumType() const
{
    return m_defaultMediumType;
}

void DicomPrinter::setAvailableMediumTypeValues(QStringList mediumTypeValues)
{
    m_mediumTypeValues = mediumTypeValues;
}

QStringList DicomPrinter::getAvailableMediumTypeValues()
{
    return m_mediumTypeValues;
}

void DicomPrinter::setDefaultFilmSize(const QString &defaultFilmSize)
{
    m_defaultFilmSize = defaultFilmSize;
}

QString DicomPrinter::getDefaultFilmSize() const
{
    return m_defaultFilmSize;
}

void DicomPrinter::setAvailableFilmSizeValues(QStringList filmSizeValues)
{
    m_filmSizeValues = filmSizeValues;
}

QStringList DicomPrinter::getAvailableFilmSizeValues()
{
    return m_filmSizeValues;
}

void DicomPrinter::setDefaultFilmLayout(const QString &_defaultFilmLayout)
{
    m_defaultFilmLayout=_defaultFilmLayout;
}

QString DicomPrinter::getDefaultFilmLayout() const
{
    return m_defaultFilmLayout;
}

int DicomPrinter::getDefaultFilmLayoutColumns()
{
    int columns, rows;

    getDefaultFilmLayoutColumnsRows(columns, rows);

    return columns;
}

int DicomPrinter::getDefaultFilmLayoutRows()
{
    int columns, rows;

    getDefaultFilmLayoutColumnsRows(columns, rows);

    return rows;
}

void DicomPrinter::setAvailableFilmLayoutValues(QStringList filmLayoutValues)
{
    m_filmLayoutValues = filmLayoutValues;
}

QStringList DicomPrinter::getAvailableFilmLayoutValues()
{
    return m_filmLayoutValues;
}

void DicomPrinter::setDefaultFilmOrientation(const QString &defaultFilmOrientation)
{
    m_defaultFilmOrientation = defaultFilmOrientation;
}

QString DicomPrinter::getDefaultFilmOrientation() const
{
    return m_defaultFilmOrientation;
}

void DicomPrinter::setAvailableFilmOrientationValues(QStringList filmOrientationValues)
{
    m_filmOrientationValues = filmOrientationValues;
}

QStringList DicomPrinter::getAvailableFilmOrientationValues()
{
    return m_filmOrientationValues;
}

void  DicomPrinter::setDefaultMagnificationType(const QString &_defaultMagnificationType)
{
    m_defaultMagnificationType=_defaultMagnificationType;
}

QString DicomPrinter::getDefaultMagnificationType() const
{
    return m_defaultMagnificationType;
}

void DicomPrinter::setAvailableMagnificationTypeValues(QStringList magnificationTypeValues)
{
    m_magnificationTypeValues = magnificationTypeValues;
}

QStringList DicomPrinter::getAvailableMagnificationTypeValues()
{
    return m_magnificationTypeValues;
}

void DicomPrinter::setDefaultMaxDensity(ushort defaultMaxDensity)
{
    m_defaultMaxDensity = defaultMaxDensity;
}

ushort DicomPrinter::getDefaultMaxDensity() const
{
    return m_defaultMaxDensity;
}

void DicomPrinter::setAvailableMaxDensityValues(ushort maxDensityValues)
{
    m_maxDensityValues = maxDensityValues;
}

ushort DicomPrinter::getAvailableMaxDensityValues()
{
    return m_maxDensityValues;
}

void DicomPrinter::setDefaultMinDensity(ushort defaultMinDensity)
{
    m_defaultMinDensity = defaultMinDensity;
}

ushort DicomPrinter::getDefaultMinDensity() const
{
    return m_defaultMinDensity;
}

void DicomPrinter::setAvailableMinDensityValues(ushort minDensityValues)
{
    m_minDensityValues = minDensityValues;
}

ushort DicomPrinter::getAvailableMinDensityValues()
{
    return m_minDensityValues;
}

void DicomPrinter::setDefaultTrim(bool _defaultTrim)
{
    m_defaultTrim=_defaultTrim;
}

bool DicomPrinter::getDefaultTrim() const
{
    return m_defaultTrim;
}

void DicomPrinter::setAvailableTrim(bool availableTrim)
{
    m_availableTrim = availableTrim;
}

bool DicomPrinter::getAvailableTrim() const
{
    return m_availableTrim;
}

void  DicomPrinter::setDefaultBorderDensity(const QString &_defaultBorderDensity)
{
    m_defaultBorderDensity=_defaultBorderDensity;
}

QString DicomPrinter::getDefaultBorderDensity() const
{
    return m_defaultBorderDensity;
}

void DicomPrinter::setAvailableBorderDensityValues(QStringList borderDensityValues)
{
    m_borderDensityValues = borderDensityValues;
}

QStringList DicomPrinter::getAvailableBorderDensityValues()
{
    return m_borderDensityValues;
}

void  DicomPrinter::setDefaultEmptyImageDensity(const QString &_defaultEmptyImageDensity)
{
    m_defaultEmptyImageDensity=_defaultEmptyImageDensity;
}

QString DicomPrinter::getDefaultEmptyImageDensity() const
{
    return m_defaultEmptyImageDensity;
}

void DicomPrinter::setAvailableEmptyImageDensityValues(QStringList emptyImageDensityValues)
{
    m_emptyImageDensityValues = emptyImageDensityValues;
}

QStringList DicomPrinter::getAvailableEmptyImageDensityValues()
{
    return m_emptyImageDensityValues;
}

void  DicomPrinter::setDefaultFilmDestination(const QString &_defaultFilmDestination)
{
    m_defaultFilmDestination=_defaultFilmDestination;
}

QString DicomPrinter::getDefaultFilmDestination() const
{
    return m_defaultFilmDestination;
}

void DicomPrinter::setAvailableFilmDestinationValues(QStringList filmDestinationValues)
{
    m_filmDestinationValues = filmDestinationValues;
}

QStringList DicomPrinter::getAvailableFilmDestinationValues()
{
    return m_filmDestinationValues;
}

void DicomPrinter::setDefaultSmoothingType(const QString &defaultSmoothingType)
{
    m_defaultSmoothingType = defaultSmoothingType;
}

QString DicomPrinter::getDefaultSmoothingType() const
{
    return m_defaultSmoothingType;
}

void DicomPrinter::setAvailableSmoothingTypeValues(QStringList smoothingTypeValues)
{
    m_smoothingTypeValues = smoothingTypeValues;
}

QStringList DicomPrinter::getAvailableSmoothingTypeValues()
{
    return m_smoothingTypeValues;
}

void DicomPrinter::setDefaultPrintPriority(const QString &defaultPrintPriority)
{
    m_defaultPrintPriority = defaultPrintPriority;
}

QString DicomPrinter::getDefaultPrintPriority() const
{
    return m_defaultPrintPriority;
}

void DicomPrinter::setAvailablePrintPriorityValues(QStringList printPriorityValues)
{
    m_printPriorityValues = printPriorityValues;
}

QStringList DicomPrinter::getAvailablePrintPriorityValues()
{
    return m_printPriorityValues;
}

void DicomPrinter::setDefaultPolarity(const QString &defaultPolarity)
{
    m_defaultPolarity = defaultPolarity;
}

QString DicomPrinter::getDefaultPolarity() const
{
    return m_defaultPolarity;
}

void DicomPrinter::setAvailablePolarityValues(QStringList polarityValues)
{
    m_polarityValues = polarityValues;
}

QStringList DicomPrinter::getAvailablePolarityValues()
{
    return m_polarityValues;
}

void DicomPrinter::setDefaultConfigurationInformation(const QString configurationInformation)
{
    m_configurationInformation = configurationInformation;
}

QString DicomPrinter::getDefaultConfigurationInformation() const
{
    return m_configurationInformation;
}

void DicomPrinter::setIsDefault(const bool &isDefault)
{
    m_isDefaultPrinter=isDefault;
}

bool DicomPrinter::getIsDefault() const
{
    return m_isDefaultPrinter;
}

void DicomPrinter::getDefaultFilmLayoutColumnsRows(int &columns, int &rows)
{
    QStringList splittedFilmLayout;
    QString filmLayout = getDefaultFilmLayout();
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