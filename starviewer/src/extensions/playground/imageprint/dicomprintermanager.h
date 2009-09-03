#ifndef UDGDICOMPRINTERMANAGER_H
#define UDGDICOMPRINTERMANAGER_H

#include <QList>
#include "printermanager.h"

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

    void addPrinter(Printer &_printer);
    void updatePrinter(Printer &_printer);
    void removePrinter(Printer &_printer);
    void removePrinter(QString &_reference);
    DicomPrinter  getPrinterByID(const int &id);
    DicomPrinter getAvailableParametersValues(const QString &AETitlePrinter, const int &port);
                                                            
private:
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
