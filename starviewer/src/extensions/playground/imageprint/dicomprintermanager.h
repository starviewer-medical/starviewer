#ifndef UDGDICOMPRINTERMANAGER_H
#define UDGDICOMPRINTERMANAGER_H

#include <QList>
#include "printermanager.h"
#include "settings.h"

/**
* Implementació per la Interfície PrinterHandler per impressores Dicom en DCMTK.
*/

namespace udg
{
    class Printer;
    class DicomPrinter;

class DicomPrinterManager: public PrinterManager
{
public:
    DicomPrinterManager();
    ~DicomPrinterManager();

    bool addPrinter(DicomPrinter &printer);
    bool updatePrinter(const int &printerID, DicomPrinter &newDicomPrinter);
    void deletePrinter(const int &printerID);
    
    DicomPrinter getPrinterByID(const int &id);
    QList<DicomPrinter> getDicomPrinterList();
    DicomPrinter getAvailableParametersValues(const QString &AETitlePrinter, const int &port);
                                                            
private:
    
    // Configuració dels Settings
    static const QString DicomPrinterListSectionName;
    Settings::KeyValueMapType dicomPrinterToKeyValueMap( const DicomPrinter &dicomPrinter);
    DicomPrinter keyValueMapToDicomPrinter(const Settings::KeyValueMapType &item);
    int indexOfPrinterInSettings(DicomPrinter &printer);
    
    QStringList getAvailableMediumTypeValues();
    QStringList getAvailableFilmSizeValues();
    QStringList getAvailableFilmLayoutValues();	
    QStringList getAvailableFilmOrientationValues();
    QStringList getAvailableMagnificationTypeValues();
    ushort getAvailableMaxDensityValues();
    ushort getAvailableMinDensityValues();
    bool getAvailableTrim() const;
    QStringList getAvailableBorderDensityValues();
    QStringList getAvailableEmptyImageDensityValues();
    QStringList getAvailableFilmDestinationValues();
    QStringList getAvailableSmoothingTypeValues();
    QStringList getAvailablePrintPriorityValues();
    QStringList getAvailablePolarityValues();
};
}; 
#endif
