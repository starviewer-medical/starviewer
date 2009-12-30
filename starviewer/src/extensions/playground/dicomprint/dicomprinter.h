#ifndef UDGDICOMPRINTER_H
#define UDGDICOMPRINTER_H

#include <QString>
#include <QStringList>
#include <QList>
#include "printer.h"

/** 
  * Implementació de la interfície Printer per una impressora del tipus Dicom
  */

namespace udg
{

class DicomPrinter : public Printer
{
public:

    DicomPrinter();

    ~DicomPrinter();

    /// Afegeix l'ID de l'impressora    
    void setID(int ID);

    /// Retorna l'ID de l'impressora    
    int getID();

    /// Afegeix el nom de la impressora.
    void setName( const QString& name );

    /// Retorna el nom de la impressora.
    QString getName() const;

    /// Afegeix el tipus de la impressora.
    void setType( const QString& type );

    /// Retorna el tipus de la impressora.
    QString getType() const;

    /// Afegeix la descripció de la impressora.
    void setDescription( const QString& description );	

    /// Retorna la descripció de la impressora.    
    QString getDescription() const;

    /// Afageix el AETitle de la impressora
    void setAETitle( const QString& aeTitle );

    /// Retorna el AETitle de la impressora
    QString getAETitle() const;

    /// Afegeix el HostName (IP) de la impressora.
    void setHostname( const QString& hostname );

    /// Retorna el AETitle de la impressora
    QString getHostname() const;

    /// Afegeix el Port de la impressora.
    void setPort( int port );

    /// Retorna el Port de la impressora.
    int getPort();

    /// Especifica el valor per defecte del MediumType
    void setDefaultMediumType( const QString& mediumType );

    /// Retorna el valor per defecte del MediumType
    QString getDefaultMediumType() const; 

    /// Especifica una llista de valors disponibles del MediumType
    void setAvailableMediumTypeValues( QStringList mediumTypeValues );

    /// Retorna una llista de valors disponibles del MediumType
    QStringList getAvailableMediumTypeValues();

    /// Especifica el valor per defecte del FilmSize
    void setDefaultFilmSize( const QString& filmSize );	

    /// Retorna el valor per defecte del FilmSize
    QString getDefaultFilmSize() const;

    /// Especifica una llista de valors disponibles del FilmSize
    void setAvailableFilmSizeValues( QStringList filmSizeValues );

    /// Retorna una llista de valors disponibles del FilmSize
    QStringList getAvailableFilmSizeValues();

     /// Especifica el valor per defecte del FilmLayout
    void setDefaultFilmLayout( const QString& filmLayout );	

    /// Retorna el valor per defecte del FilmLayout
    QString getDefaultFilmLayout() const;

    /// Especifica una llista de valors disponibles del FilmLayout
    void setAvailableFilmLayoutValues(QStringList filmLayoutValues);	

    /// Retorna una llista de valors disponibles del FilmLayout
    QStringList getAvailableFilmLayoutValues();	
        
     /// Retorna el número de columnes que té el layout del Film per defecte de la impressora.
    int getDefaultFilmLayoutColumns();

    /// Retorna el número de files que té el layout del Film per defecte de la impressora.
    int getDefaultFilmLayoutRows();

    /// Especifica la orientació (horitzontal/vertical) per defecte amb la que s'ha d'imprimir la placa
    void setDefaultFilmOrientation(const QString &filmOrientation);	

    /// Retorna la orientació (horitzontal/vertical) per defecte amb la que s'ha d'imprimir la placa
    QString getDefaultFilmOrientation() const;

    /// Especifica la llista d'orientacions (horitzontal/vertical) disponibles amb la que s'ha d'imprimir la placa
    void setAvailableFilmOrientationValues(QStringList filmOrientationValues);

     /// Retorna la llista d'orientacions (horitzontal/vertical) disponibles amb les que es pot imprimir la placa
    QStringList getAvailableFilmOrientationValues();

    /// Afegeix el Magnification Type per defecte a la impressora
    void setDefaultMagnificationType( const QString& magnificationType );	

    /// Retorna el Magnification Type per defecte a la impressora
    QString getDefaultMagnificationType() const;

    /// Afegeix una llista de Magnification Type disponibles a la impressora
    void setAvailableMagnificationTypeValues(QStringList magnificationTypeValues);

    /// Retorna una llista de Magnification Type disponibles a la impressora
    QStringList getAvailableMagnificationTypeValues();

    /// Afegeix la Densitat Màxima per defecte a la impressora
    void setDefaultMaxDensity( ushort maxDensity );

    /// Retorna la Densitat Màxima per defecte a la impressora
    ushort getDefaultMaxDensity() const;

    /// Afegeix la Densitat Màxima que pot agafar la impressora
    void setAvailableMaxDensityValues(ushort maxDensityValues);

    /// Retorna la Densitat Màxima que pot agafar la impressora
    ushort getAvailableMaxDensityValues();

    /// Especifica la resolució mínima de la impressió. Si té valor 0 vol dir que no s'aplica
    void setDefaultMinDensity(ushort minDensity);

    /// Retorna la resolució mínima de la impressió. Si té valor 0 vol dir que no s'aplica
    ushort getDefaultMinDensity() const;

    /// Especifica la resolució mínima que pot agafar la impressió. Si té valor 0 vol dir que no s'aplica
    void setAvailableMinDensityValues(ushort minDensityValues);

    /// Retorna la resolució mínima que pot agafar la impressió. Si té valor 0 vol dir que no s'aplica
    ushort getAvailableMinDensityValues();

    /// Especifica si s'accepta TrimBox per defecte
    void setDefaultTrim(bool trim);

    /// Retorna si s'accepta TrimBox per defecte
    bool getDefaultTrim();

    /// Especifica si s'accepta TrimBox.
    void setAvailableTrim(bool availableTrim);

    /// Retorna si s'accepta TrimBox.
    bool getAvailableTrim();

    /// Especifica el valor per defecte de la Densitat del Border.
    void setDefaultBorderDensity(const QString &borderDensity);

    /// Retorna el valor per defecte de la Densitat del Border.
    QString getDefaultBorderDensity() const;

    /// Especifica una llista de  valors disponibles de la Densitat del Border.
    void setAvailableBorderDensityValues(QStringList borderDensityValues);

    /// Retorna una llista de  valors disponibles de la Densitat del Border.
    QStringList getAvailableBorderDensityValues();

    /// Especifica el valor per defecte de l'EmptyImageDensity
    void setDefaultEmptyImageDensity(const QString &emptyImageDensity);

    /// Retorna el valor per defecte de l'EmptyImageDensity
    QString getDefaultEmptyImageDensity() const;

    /// Especifica una llista de  valors disponibles de l'EmptyImageDensity    
    void setAvailableEmptyImageDensityValues(QStringList emptyImageDensityValues);

    /// Retorna una llista de  valors disponibles de l'EmptyImageDensity    
    QStringList getAvailableEmptyImageDensityValues();

    /// Especifica el valor per defecte del FilmDestination
    void setDefaultFilmDestination(const QString &filmDestination);

    /// Retorna el valor per defecte del FilmDestination
    QString getDefaultFilmDestination() const;

    /// Especifica una llista de  valors disponibles del FilmDestination    
    void setAvailableFilmDestinationValues(QStringList filmDestinationValues);

    /// Retorna una llista de  valors disponibles del FilmDestination
    QStringList getAvailableFilmDestinationValues();

    ///Especifica el Smoothing type per defecte amb la que s'imprimirà
    void setDefaultSmoothingType(const QString &smoothingType);

    /// Retorna el Smoothing type per defecte amb la que s'imprimirà
    QString getDefaultSmoothingType() const;

    /// Especifica una llista de Smoothing type disponibles amb la que es pot imprimir.
    void setAvailableSmoothingTypeValues(QStringList smoothingTypeValues);

    /// Retorna una llista de Smoothing type disponibles amb la que es pot imprimir.
    QStringList getAvailableSmoothingTypeValues();

    /// Especifica la prioritat per defecte dels jobs que s'imprimeixin amb aquesta impressora
    void setDefaultPrintPriority(const QString& priority);

    /// Retorna la prioritat per defecte dels jobs que s'imprimeixin amb aquesta impressora
    QString getDefaultPrintPriority() const;

    /// Especifica una llista de prioritats disponibles dels jobs que s'imprimeixin amb aquesta impressora
    void setAvailablePrintPriorityValues(QStringList printPriorityValues);

    /// Retorna una llista de prioritats disponibles dels jobs que s'imprimeixin amb aquesta impressora
    QStringList getAvailablePrintPriorityValues();

    /// Especifica la polaritat per defecte de l'impressora
    void setDefaultPolarity(const QString& polarity);

    /// Retorna la polaritat per defecte de l'impressora
    QString getDefaultPolarity() const;

    /// Especifica una llista de polaritats per defecte de l'impressora
    void setAvailablePolarityValues(QStringList polarityValues);

    /// Retorna una llista de polaritats per defecte de l'impressora
    QStringList getAvailablePolarityValues();

    /**Especifica/retorna el valor de ConfigurationInformation per la impressora, depén de la impressora, per exemple per agfa permet afegir 
      *anotacions a les imatges, aplicar LUT's, ... s'ha de mirar al dicom confomance de cada impressora per mirar quin valor pot agafar aquest camp
      */
    //El configuration Information no té AvailableConfiguration mètodes, perquè per cada impressora el valor és diferent.
    void setDefaultConfigurationInformation(const QString configurationInformation);
    QString getDefaultConfigurationInformation() const;

    /// Especifica si la impressora és la impressora per defecte del sistema.
    void setIsDefault( bool isDefault );

    /// Retorna si la impressora és la impressora per defecte del sistema.
    bool getIsDefault() const;

private:
    
    /**Mètode que retorna el número de columnes i files del layout, s'implementa en un sol mètode per no tenir codi duplicat
    si s'implementés un mètode per obtenir el número de columnes i una altre per obtenir el número de files*/
    void getDefaultFilmLayoutColumnsRows(int &colums, int &rows);   

private:		
	
    int m_ID;
    QString m_aeTitle;
    QString m_hostname;
    int m_port;
    QString	m_defaultDescription;
    QString	m_defaultMediumType;
    QStringList m_mediumTypeValues;
    QString	m_defaultFilmSize;
    QStringList m_filmSizeValues;
    QString	m_defaultFilmLayout;
    QStringList m_filmLayoutValues;
    QString m_defaultFilmOrientation;
    QStringList m_filmOrientationValues;        
    QString	m_defaultMagnificationType;
    QStringList m_magnificationTypeValues;
    ushort m_defaultMaxDensity;
    ushort m_maxDensityValues;
    ushort m_defaultMinDensity;
    ushort m_minDensityValues;
    bool m_defaultTrim;
    bool m_availableTrim;
    QString m_defaultBorderDensity;
    QStringList m_borderDensityValues;
    QString m_defaultEmptyImageDensity;
    QStringList m_emptyImageDensityValues;
    QString m_defaultFilmDestination;
    QStringList m_filmDestinationValues;
    QString m_defaultSmoothingType;
    QStringList m_smoothingTypeValues;
    QString m_defaultPrintPriority;
    QStringList m_printPriorityValues;
    QString m_defaultPolarity;
    QStringList m_polarityValues;    
    QString m_configurationInformation;
};
}; 
#endif
