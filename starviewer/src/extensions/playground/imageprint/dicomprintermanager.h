#ifndef UDGDICOMPRINTERMANAGER_H
#define UDGDICOMPRINTERMANAGER_H

#include <QList>
#include "printermanager.h"
#include "settings.h"

/**
* Implementació de la interfície PrinterManager per impressores DICOM en DCMTK.
* Aquest classe permet Afegir/Modificar/Esborrar les impressores DICOM que tenim guardades al sistema mitjançant Settings.
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

    /** Afegeix una impressora Dicom (\p printer) al sistema.
        Retorna True si s'ha pogut afegir la impressora i False si la impressora ja existeix al sistema. */
    bool addPrinter( DicomPrinter &printer );
    
    /** Modifica la impressora amb identificador printerID amb els nous paràmetres guardats a newDicomPrinter.
        Retorna True si s'ha pogut modificar correctament i False si la impressora a modificar no existeix o no és vàlida. */    
    bool updatePrinter( int &printerID, DicomPrinter &newDicomPrinter );
    
    /// Esborra la impressora amb identificador printerID.
    void deletePrinter( int &printerID );
    
    /// Retorna un objecte DicomPrinter que conté les dades de la impressora amb identificador printerID.
    DicomPrinter getPrinterByID( int pritnerID );
    
    /// Retorna una llista de DicomPrinters que hi ha emmagatzemades al sistema.
    QList<DicomPrinter> getDicomPrinterList();

    /** Retorna un objecte DicomPrinter amb les dades de la impressora que té com a AETitle el valor AETitlePrinter
        i com a Port el valor portPrinter. 
        TO-DO: Ara mateix et retorna valors per defecte ja que no es comunica amb la impressora. Falta realitza la implementació 
        per obtenir les dades directament de la impressora.*/
    DicomPrinter getAvailableParametersValues( const QString &AETitlePrinter, int portPrinter );
                                                            
private:

    /// Retorna un Settings::KeyValueMapType omplert amb les dades de la impressora dicomPrinter.
    Settings::KeyValueMapType dicomPrinterToKeyValueMap( DicomPrinter &dicomPrinter );

    /// Retorna un DicomPrinter omplert amb les dades que conté el Settings::KeyValueMapType.
    DicomPrinter keyValueMapToDicomPrinter( Settings::KeyValueMapType &item);

    /// Retorna l'index on està emmagatzemada la impressora printer dins del sistema (Settings).
    int indexOfPrinterInSettings(DicomPrinter &printer);

    /// Retorna una llista de valors  que pot agafar el MediumType d'una impressora Dicom.
    QStringList getAvailableMediumTypeValues();

    /// Retorna una llista de possibles valors que pot agafar el FilmSize d'una impressora Dicom.
    QStringList getAvailableFilmSizeValues();

    /// Retorna una llista de possibles valors que pot agafar el FilmLayout d'una impressora Dicom.
    QStringList getAvailableFilmLayoutValues();	

    /// Retorna una llista de possibles valors que pot agafar el FilmOrentation d'una impressora Dicom.
    QStringList getAvailableFilmOrientationValues();

    /// Retorna una llista de possibles valors que pot agafar el MagnificationType d'una impressora Dicom.
    QStringList getAvailableMagnificationTypeValues();

    /// Retorna el valor màxim de densitat que pot agafar una impressora Dicom.
    ushort getAvailableMaxDensityValues();

    /// Retorna el valor mínim de densitat que pot agafar una impressora Dicom.
    ushort getAvailableMinDensityValues();

    /// Retorna la possibilitat d'utilitzar Trim en una impressora Dicom.
    bool getAvailableTrim();

    /// Retorna una llista de possibles valors que pot agafar el MagnificationType d'una impressora Dicom.
    QStringList getAvailableBorderDensityValues();

    /// Retorna una llista de possibles valors que pot agafar el EmptyImageDensity d'una impressora Dicom.
    QStringList getAvailableEmptyImageDensityValues();

    /// Retorna una llista de possibles valors que pot agafar el FilmDestination d'uma impressora Dicom.
    QStringList getAvailableFilmDestinationValues();

    /// Retorna una llista de possibles valors que pot agafar el SmoothingType d'uma impressora Dicom.
    QStringList getAvailableSmoothingTypeValues();

    /// Retorna una llista de possibles valors que pot agafar el PrintPriority d'uma impressora Dicom.
    QStringList getAvailablePrintPriorityValues();

    /// Retorna una llista de possibles valors que pot agafar el Polarity d'uma impressora Dicom.
    QStringList getAvailablePolarityValues();

private:

    /// Conté el nom de la secció del Settings on es guarden les dades de la impressora.
    static const QString DicomPrinterListSectionName;
};
}; 
#endif