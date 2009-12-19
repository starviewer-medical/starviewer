#include <QString>
#include <QSettings>

#include "printer.h"
#include "printermanager.h"
#include "dicomprintermanager.h"
#include "dicomprinter.h"
#include "settings.h"

namespace udg 
{	

const QString DicomPrinterManager::DicomPrinterListSectionName = "DicomPrinterList";

DicomPrinterManager::DicomPrinterManager()
{

}
 
DicomPrinterManager::~DicomPrinterManager()
{

}

bool DicomPrinterManager::addPrinter( DicomPrinter &printer )
{   
    if ( this->indexOfPrinterInSettings( printer ) == -1 )
    {
        Settings settings;
        settings.addListItem( DicomPrinterListSectionName, dicomPrinterToKeyValueMap(printer) );
        printer.setID( this->getDicomPrinterList().count() - 1 );
        return true;
    }
    return false;
}

bool DicomPrinterManager::updatePrinter( int &printerID, DicomPrinter &printer )
{
    Settings settings;
    int indexTrobat = this->indexOfPrinterInSettings(printer);
    if( indexTrobat == printerID || indexTrobat==-1 )
    {
        settings.setListItem( printerID, DicomPrinterListSectionName, dicomPrinterToKeyValueMap(printer) );    
        return true;
    }
    else
    {
        return false;
    }
}

void DicomPrinterManager::deletePrinter( int &printerID )
{
    Settings settings;
    settings.removeListItem( DicomPrinterListSectionName, printerID );
}

DicomPrinter DicomPrinterManager::getPrinterByID( int id )
{   
    DicomPrinter dicomPrinter;
    QList<DicomPrinter> dicomPrinterList = getDicomPrinterList();    
    if ( id < dicomPrinterList.count() ) 
    {
       dicomPrinter = dicomPrinterList.at(id);
    }
        
    dicomPrinter.setAvailableMediumTypeValues( this->getAvailableMediumTypeValues() );
    dicomPrinter.setAvailableFilmSizeValues( this->getAvailableFilmSizeValues() );
    dicomPrinter.setAvailableFilmLayoutValues( this->getAvailableFilmLayoutValues() );
    dicomPrinter.setAvailableFilmOrientationValues( this->getAvailableFilmOrientationValues() );
    dicomPrinter.setAvailableMagnificationTypeValues( this->getAvailableMagnificationTypeValues() );
    dicomPrinter.setAvailableMaxDensityValues( this->getAvailableMaxDensityValues() );
    dicomPrinter.setAvailableMinDensityValues( this->getAvailableMinDensityValues() );
    dicomPrinter.setAvailableTrim( this->getAvailableTrim() );
    dicomPrinter.setAvailableBorderDensityValues( this->getAvailableBorderDensityValues() );
    dicomPrinter.setAvailableEmptyImageDensityValues( this->getAvailableEmptyImageDensityValues() );
    dicomPrinter.setAvailableFilmDestinationValues( this->getAvailableFilmDestinationValues() );
    dicomPrinter.setAvailableSmoothingTypeValues( this->getAvailableSmoothingTypeValues() );
    dicomPrinter.setAvailablePrintPriorityValues( this->getAvailablePrintPriorityValues() );
    dicomPrinter.setAvailablePolarityValues( this->getAvailablePolarityValues() );

    return dicomPrinter;
}

QList<DicomPrinter> DicomPrinterManager::getDicomPrinterList()
{
    Settings settings;
    QList<DicomPrinter> dicomPrintersList;
    int indexOfIDPrinter = 0;

    Settings::SettingListType list = settings.getList(DicomPrinterListSectionName);
    
    foreach( Settings::KeyValueMapType item, list )
    {
        DicomPrinter dicomPrinter;

        dicomPrinter = keyValueMapToDicomPrinter(item);
        dicomPrinter.setID(indexOfIDPrinter);
        dicomPrintersList << dicomPrinter;
        indexOfIDPrinter++;
    }
    return dicomPrintersList;   
}


//TODO: Ara mateix et retorna valors per defecte ja que no es comunica amb la impressora. Falta realitza la implementació per obtenir les dades
// directament de la impressora.
DicomPrinter  DicomPrinterManager::getAvailableParametersValues(const QString &AETitlePrinter, int portPrinter)
{
    DicomPrinter dicomPrinter;
    
    dicomPrinter.setAvailableMediumTypeValues( this->getAvailableMediumTypeValues() );
    dicomPrinter.setAvailableFilmSizeValues( this->getAvailableFilmSizeValues() );
    dicomPrinter.setAvailableFilmLayoutValues( this->getAvailableFilmLayoutValues() );
    dicomPrinter.setAvailableFilmOrientationValues( this->getAvailableFilmOrientationValues() );
    dicomPrinter.setAvailableMagnificationTypeValues( this->getAvailableMagnificationTypeValues() );
    dicomPrinter.setAvailableMaxDensityValues( this->getAvailableMaxDensityValues() );
    dicomPrinter.setAvailableMinDensityValues( this->getAvailableMinDensityValues() );
    dicomPrinter.setAvailableTrim( this->getAvailableTrim() );
    dicomPrinter.setAvailableBorderDensityValues( this->getAvailableBorderDensityValues() );
    dicomPrinter.setAvailableEmptyImageDensityValues( this->getAvailableEmptyImageDensityValues() );
    dicomPrinter.setAvailableFilmDestinationValues( this->getAvailableFilmDestinationValues() );    
    dicomPrinter.setAvailableSmoothingTypeValues( this->getAvailableSmoothingTypeValues() );    
    dicomPrinter.setAvailablePrintPriorityValues( this->getAvailablePrintPriorityValues() );
    dicomPrinter.setAvailablePolarityValues( this->getAvailablePolarityValues() );
    return dicomPrinter;
}

Settings::KeyValueMapType DicomPrinterManager::dicomPrinterToKeyValueMap( DicomPrinter &dicomPrinter )
{
    Settings::KeyValueMapType item;
    
    item["AETitle"] = dicomPrinter.getAETitle();
    item["PrinterName"] = dicomPrinter.getName();
    item["PrinterType"] = dicomPrinter.getType();
    item["PrinterDescription"] = dicomPrinter.getDescription();
    item["PrinterHostname"] = dicomPrinter.getHostname();
    item["PrinterPort"] = dicomPrinter.getPort();
    item["IsDefaultPrinter"] = dicomPrinter.getIsDefault();
    item["DefaultMediumType"] = dicomPrinter.getDefaultMediumType();
    item["DefaultFilmSize"] = dicomPrinter.getDefaultFilmSize();
    item["DefaultFilmLayout"] = dicomPrinter.getDefaultFilmLayout();
    item["DefaultFilmOrientation"] = dicomPrinter.getDefaultFilmOrientation();
    item["DefaultMagnificationType"] = dicomPrinter.getDefaultMagnificationType();
    item["DefaultMaxDensity"] = dicomPrinter.getDefaultMaxDensity();
    item["DefaultMinDensity"] = dicomPrinter.getDefaultMinDensity();
    item["DefaultTrim"] = dicomPrinter.getDefaultTrim();
    item["DefaultBorderDensity"] = dicomPrinter.getDefaultBorderDensity();
    item["DefaultEmptyImageDensity"] = dicomPrinter.getDefaultEmptyImageDensity();
    item["DefaultFilmDestination"] = dicomPrinter.getDefaultFilmDestination();
    item["DefaultSmoothingType"] = dicomPrinter.getDefaultSmoothingType();
    item["DefaultPrintPriority"] = dicomPrinter.getDefaultPrintPriority();
    item["DefaultPolarity"] = dicomPrinter.getDefaultPolarity();
    item["ConfigurationInformation"] = dicomPrinter.getDefaultConfigurationInformation();

    return item;
}

DicomPrinter DicomPrinterManager::keyValueMapToDicomPrinter( Settings::KeyValueMapType &item )
{
    DicomPrinter dicomPrinter;

    dicomPrinter.setAETitle(item.value("AETitle").toString());
    dicomPrinter.setName(item.value("PrinterName").toString());
    dicomPrinter.setType(item.value("PrinterType").toString());
    dicomPrinter.setDescription(item.value("PrinterDescription").toString());
    dicomPrinter.setHostname(item.value("PrinterHostname").toString());
    dicomPrinter.setPort(item.value("PrinterPort").toInt());
    dicomPrinter.setIsDefault(item.value("IsDefaultPrinter").toBool());
    dicomPrinter.setDefaultMediumType(item.value("DefaultMediumType").toString());
    dicomPrinter.setDefaultFilmSize(item.value("DefaultFilmSize").toString());
    dicomPrinter.setDefaultFilmLayout(item.value("DefaultFilmLayout").toString());
    dicomPrinter.setDefaultFilmOrientation(item.value("DefaultFilmOrientation").toString());
    dicomPrinter.setDefaultMagnificationType(item.value("DefaultMagnificationType").toString());
    dicomPrinter.setDefaultMaxDensity(item.value("DefaultMaxDensity").toUInt());
    dicomPrinter.setDefaultMinDensity(item.value("DefaultMinDensity").toUInt());
    dicomPrinter.setDefaultTrim(item.value("DefaultTrim").toBool());
    dicomPrinter.setDefaultBorderDensity(item.value("DefaultBorderDensity").toString());
    dicomPrinter.setDefaultEmptyImageDensity(item.value("DefaultEmptyImageDensity").toString());
    dicomPrinter.setDefaultFilmDestination(item.value("DefaultFilmDestination").toString());
    dicomPrinter.setDefaultSmoothingType(item.value("DefaultSmoothingType").toString());
    dicomPrinter.setDefaultPrintPriority(item.value("DefaultPrintPriority").toString());
    dicomPrinter.setDefaultPolarity(item.value("DefaultPolarity").toString());
    dicomPrinter.setDefaultConfigurationInformation(item.value("ConfigurationInformation").toString());
    
    return dicomPrinter;
}

int DicomPrinterManager::indexOfPrinterInSettings( DicomPrinter &printer )
{
    int i=0;       
    QList<DicomPrinter> dicomPrinterList = getDicomPrinterList();
    
    foreach(DicomPrinter printerTemp, dicomPrinterList)
    {
       if (printerTemp.getAETitle() == printer.getAETitle() &&
           printerTemp.getHostname() == printer.getHostname() &&
           printerTemp.getPort() == printer.getPort())
       {
         return i;
       }
        
       i = i+1;
    }
    return i = -1;
}

QStringList DicomPrinterManager::getAvailableMediumTypeValues()
{
    QStringList mediumTypeValuesTemp;
    mediumTypeValuesTemp << "" << "PAPER" << "CLEAR FILM" << "BLUE FILM" << "MAMMO CLEAR FILM" << "MAMMO BLUE FILM";
    return mediumTypeValuesTemp;
}

QStringList DicomPrinterManager::getAvailableFilmSizeValues()
{
    QStringList filmSizeValuesTemp;
    filmSizeValuesTemp << "" << "8INX10IN" << "8_5INX11IN" << "10INX12IN" << "10INX14IN" << "11INX14IN" << "11INX17IN" << "14INX14IN" << "14INX17IN" << "24CMX24CM" << "24CMX30CM" << "A4" << "A3";
    return filmSizeValuesTemp;
}

QStringList DicomPrinterManager::getAvailableFilmLayoutValues()
{
    QStringList filmLayoutValuesTemp;

    filmLayoutValuesTemp <<"STANDARD\\1,1" << "STANDARD\\1,2" << "STANDARD\\2,2" << "STANDARD\\2,3" << "STANDARD\\2,4" << "STANDARD\\3,3 " << "STANDARD\\3,4" 
                         << "STANDARD\\3,5" << "STANDARD\\4,4" << "STANDARD\\4,5" << "STANDARD\\4,6" << "STANDARD\\5,6" << "STANDARD\\5,7";

    return filmLayoutValuesTemp;
}

QStringList DicomPrinterManager::getAvailableFilmOrientationValues()
{
    QStringList filmOrientationValuesTemp;
    filmOrientationValuesTemp << "" <<"PORTRAIT" << "LANDSCAPE";
    return filmOrientationValuesTemp;
}

QStringList DicomPrinterManager::getAvailableMagnificationTypeValues()
{
    //Atenció si es canvien els valors de Magnification Type, perquè Smoothing Type només es pot configurar si MagnificationType té com a valor "CUBINC" 
    QStringList magnificationTypeValuesTemp;
    magnificationTypeValuesTemp << "" << "REPLICATE" << "BILINEAR" << "CUBIC" << "NONE";
    return magnificationTypeValuesTemp;
}

ushort DicomPrinterManager::getAvailableMaxDensityValues()
{
    ushort maxDensityValuesTemp = 1000;
    return maxDensityValuesTemp;
}

ushort DicomPrinterManager::getAvailableMinDensityValues()
{
    ushort minDensityValues = 1000;
    return minDensityValues;
}

bool DicomPrinterManager::getAvailableTrim()
{
    return true;
}

QStringList DicomPrinterManager::getAvailableBorderDensityValues()
{
    QStringList borderDensityValuesTemp;
    borderDensityValuesTemp << "" << "BLACK" << "WHITE";
    return borderDensityValuesTemp;
}

QStringList DicomPrinterManager::getAvailableEmptyImageDensityValues()
{
    QStringList emptyImageDensityValuesTemp;
    emptyImageDensityValuesTemp << "" << "BLACK" << "WHITE";
    return emptyImageDensityValuesTemp;
}

QStringList DicomPrinterManager::getAvailableFilmDestinationValues()
{
    QStringList filmDestinationValuesTemp;
    filmDestinationValuesTemp << "" << "MAGAZINE" << "PROCESSOR" << "BIN_1" << "BIN_2" << "BIN_3" << "BIN_4" << "BIN_5" << "BIN_6" << "BIN_7" 
                              << "BIN_8" << "BIN_9" << "BIN_10";
    return filmDestinationValuesTemp;
}

QStringList DicomPrinterManager::getAvailableSmoothingTypeValues()
{
    //Només es pot configurar si Magnification Type té com a Valor "CUBIC"
    QStringList smoothingTypeValuesTemp;
    smoothingTypeValuesTemp << "" << "MEDIUM" << "SHARP" << "SMOOTH";
    return smoothingTypeValuesTemp;
}

QStringList DicomPrinterManager::getAvailablePrintPriorityValues()
{
    QStringList printPriorityValuesTemp;
    printPriorityValuesTemp << "" << "HIGH" << "MED" << "LOW";
    return printPriorityValuesTemp;
}

QStringList DicomPrinterManager::getAvailablePolarityValues()
{
    QStringList polarityValuesTemp;
    polarityValuesTemp << "" << "NORMAL" << "REVERSE";
    return polarityValuesTemp;
}
}