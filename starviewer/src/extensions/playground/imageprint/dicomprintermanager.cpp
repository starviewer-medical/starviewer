#include <QString>
#include <QSettings>

#include "printer.h"
#include "printermanager.h"
#include "dicomprintermanager.h"
#include "dicomprinter.h"

namespace udg 
{	
DicomPrinterManager::DicomPrinterManager()
{	
}

 
DicomPrinterManager::~DicomPrinterManager()
{
}

void DicomPrinterManager::addPrinter(Printer &_printer)
{	

}

void DicomPrinterManager::updatePrinter(Printer &_printer)
{

}

void DicomPrinterManager::removePrinter(Printer &_printer)
{

}

void DicomPrinterManager::removePrinter(QString &_reference)
{ 
	
}

DicomPrinter DicomPrinterManager::getPrinterByID(const int &id)
{
    DicomPrinter dicomPrinter;

    //dicomPrinter.setAETitle(m_settings.get("PrinterAETitle"));
    //dicomPrinter.setName(m_settings.get("PrinterName"));
    //dicomPrinter.setType(m_settings.get("PrinterType"));
    //dicomPrinter.setDescription(m_settings.get("PrinterDescription"));
    //dicomPrinter.setHostname(m_settings.get("PrinterHostname"));
    //dicomPrinter.setPort(m_settings.get("PrinterPort"));
    dicomPrinter.setAvailableMediumTypeValues(this->getAvailableMediumTypeValues());
    //dicomPrinter.setDefaultMediumType(m_settings.get("DefaultMediumType"));
    dicomPrinter.setAvailableFilmSizeValues(this->getAvailableFilmSizeValues());
    //dicomPrinter.setDefaultFilmSize(m_settings.get("DefaultFilmSize"));
    dicomPrinter.setAvailableFilmLayoutValues(this->getAvailableFilmLayoutValues());
    //dicomPrinter.setDefaultFilmLayout(m_settings.get("DefaultFilmLayout"));
    dicomPrinter.setAvailableFilmOrientationValues(this->getAvailableFilmOrientationValues());
    //dicomPrinter.setDefaultFilmOrientation(m_settings.get("DefaultFilmOrientation"));
    dicomPrinter.setAvailableMagnificationTypeValues(this->getAvailableMagnificationTypeValues());
    //dicomPrinter.setDefaultMagnificationType(m_settings.get("DefaultMagnificationType"));
    dicomPrinter.setAvailableMaxDensityValues(this->getAvailableMaxDensityValues());
    //dicomPrinter.setDefaultMaxDensity(m_settings.get("DefaultMaxDensity"));
    dicomPrinter.setAvailableMinDensityValues(this->getAvailableMinDensityValues());
    //dicomPrinter.setDefaultMinDensity(m_settings.get("DefaultMinDensity"));   
    dicomPrinter.setAvailableTrim(this->getAvailableTrim());
    //dicomPrinter.setDefaultTrim(m_settings.get("DefaultTrim"));   
    dicomPrinter.setAvailableBorderDensityValues(this->getAvailableBorderDensityValues());
    //dicomPrinter.setDefaultBorderDensity(m_settings.get("DefaultBorderDensity"));   
    dicomPrinter.setAvailableEmptyImageDensityValues(this->getAvailableEmptyImageDensityValues());
    //dicomPrinter.setDefaultEmptyImageDensity(m_settings.get("DefaultEmptyImageDensity"));   
    dicomPrinter.setAvailableFilmDestinationValues(this->getAvailableFilmDestinationValues());
    //dicomPrinter.setDefaultFilmDestination(m_settings.get("DefaultFilmDestination"));   
    dicomPrinter.setAvailableSmoothingTypeValues(this->getAvailableSmoothingTypeValues());
    //dicomPrinter.setDefaultSmoothingType(m_settings.get("DefaultSmoothingType"));   
    dicomPrinter.setAvailablePrintPriorityValues(this->getAvailablePrintPriorityValues());
    //dicomPrinter.setDefaultPrintPriority(m_settings.get("DefaultPrintPriority"));   
    dicomPrinter.setAvailablePolarityValues(this->getAvailablePolarityValues());
    //dicomPrinter.setDefaultPolarity(m_settings.get("DefaultPolarity"));   

    return dicomPrinter;
}

//TODO: Ara mateix et retorna valors per defecte ja que no es comunica amb la impressora. Falta realitza la implementació per obtenir les dades
// directament de la impressora.
DicomPrinter  DicomPrinterManager::getAvailableParametresValues(const QString &AETitlePrinter, const int &port)
{
    DicomPrinter dicomPrinter;
    
    dicomPrinter.setAvailableMediumTypeValues(this->getAvailableMediumTypeValues());
    dicomPrinter.setAvailableFilmSizeValues(this->getAvailableFilmSizeValues());
    dicomPrinter.setAvailableFilmLayoutValues(this->getAvailableFilmLayoutValues());
    dicomPrinter.setAvailableFilmOrientationValues(this->getAvailableFilmOrientationValues());
    dicomPrinter.setAvailableMagnificationTypeValues(this->getAvailableMagnificationTypeValues());
    dicomPrinter.setAvailableMaxDensityValues(this->getAvailableMaxDensityValues());
    dicomPrinter.setAvailableMinDensityValues(this->getAvailableMinDensityValues());
    dicomPrinter.setAvailableTrim(this->getAvailableTrim());
    dicomPrinter.setAvailableBorderDensityValues(this->getAvailableBorderDensityValues());
    dicomPrinter.setAvailableEmptyImageDensityValues(this->getAvailableEmptyImageDensityValues());
    dicomPrinter.setAvailableFilmDestinationValues(this->getAvailableFilmDestinationValues());    
    dicomPrinter.setAvailableSmoothingTypeValues(this->getAvailableSmoothingTypeValues());    
    dicomPrinter.setAvailablePrintPriorityValues(this->getAvailablePrintPriorityValues());
    dicomPrinter.setAvailablePolarityValues(this->getAvailablePolarityValues());
    return dicomPrinter;
}

QStringList DicomPrinterManager::getAvailableMediumTypeValues()
{
    QStringList mediumTypeValuesTemp;
    mediumTypeValuesTemp << "PAPER" << "CLEAR FILM" << "BLUE FILM" << "MAMMO CLEAR FILM" << "MAMMO BLUE FILM";
    return mediumTypeValuesTemp;
}

QStringList DicomPrinterManager::getAvailableFilmSizeValues()
{
    QStringList filmSizeValuesTemp;
    filmSizeValuesTemp << "8INX10IN" << "8_5INX11IN" << "10INX12IN" << "10INX14IN" << "11INX14IN" << "11INX17IN" << "14INX14IN" << "14INX17IN" << "24CMX24CM" << "24INX30IN" << "A4" << "A3";
    return filmSizeValuesTemp;
}

QStringList DicomPrinterManager::getAvailableFilmLayoutValues()
{
    QStringList filmLayoutValuesTemp;
    filmLayoutValuesTemp <<"1,1" << "1,2" << "2,2" << "2,3" << "3,3 " << "3,4" << "3,5" << "4,4" << "4,5";
    return filmLayoutValuesTemp;
}

QStringList DicomPrinterManager::getAvailableFilmOrientationValues()
{
    QStringList filmOrientationValuesTemp;
    filmOrientationValuesTemp <<"PORTRAIT" << "LANDSCAPE";
    return filmOrientationValuesTemp;
}

QStringList DicomPrinterManager::getAvailableMagnificationTypeValues()
{
    QStringList magnificationTypeValuesTemp;
    magnificationTypeValuesTemp << "REPLICATE" << "BILINEAR" << "CUBIC" << "NONE";
    return magnificationTypeValuesTemp;
}

ushort DicomPrinterManager::getAvailableMaxDensityValues()
{
    ushort maxDensityValuesTemp = 320;
    return maxDensityValuesTemp;
}

ushort DicomPrinterManager::getAvailableMinDensityValues()
{
    ushort minDensityValues = 20;
    return minDensityValues;
}

bool DicomPrinterManager::getAvailableTrim() const
{
    return true;
}

QStringList DicomPrinterManager::getAvailableBorderDensityValues()
{
    QStringList borderDensityValuesTemp;
    borderDensityValuesTemp << "150" << "20" << "BLACK" << "WHITE";
    return borderDensityValuesTemp;
}

QStringList DicomPrinterManager::getAvailableEmptyImageDensityValues()
{
    QStringList emptyImageDensityValuesTemp;
    emptyImageDensityValuesTemp << "20" << "BLACK" << "WHITE";
    return emptyImageDensityValuesTemp;
}

QStringList DicomPrinterManager::getAvailableFilmDestinationValues()
{
    QStringList filmDestinationValuesTemp;
    filmDestinationValuesTemp << "MAGAZINE" << "PROCESSOR" << "BIN_1";
    return filmDestinationValuesTemp;
}

QStringList DicomPrinterManager::getAvailableSmoothingTypeValues()
{
    QStringList smoothingTypeValuesTemp;
    smoothingTypeValuesTemp << "MEDIUM" << "SHARP" << "SMOOTH";
    return smoothingTypeValuesTemp;
}

QStringList DicomPrinterManager::getAvailablePrintPriorityValues()
{
    QStringList printPriorityValuesTemp;
    printPriorityValuesTemp << "HIGH" << "MED" << "LOW";
    return printPriorityValuesTemp;
}

QStringList DicomPrinterManager::getAvailablePolarityValues()
{
    QStringList polarityValuesTemp;
    polarityValuesTemp << "NORMAL" << "REVERSE";
    return polarityValuesTemp;
}
}