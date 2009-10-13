#ifndef UDGDICOMPRINTER_H
#define UDGDICOMPRINTER_H

#include <QString>
#include <QStringList>
#include <QList>
#include "printer.h"

///Representació dels paràmetres d'una impressora DICOM.

namespace udg
{   

class DicomPrinter : public Printer
{
public:
    DicomPrinter();
    ~DicomPrinter();
    
    /// Posa el nom de la impressora.
    void setName(const QString &name);
    QString getName() const;

    /// Posa el tipus de la impressora.
    void setType(const QString &type);
    QString getType() const;

    /// Posa la descripció de la impressora.
    void setDescription(const QString &description);	
    QString getDescription() const;

    ///Especifica/retorna el AETitle de la impressora
    void setAETitle(const QString &aeTitle);
    QString getAETitle() const;

    /// Posa el HostName (IP) de la impressora.
    void setHostname(const QString &hostname);
    QString getHostname() const;

    /// Posa el Port de la impressora.
    void setPort(const int &port);
    int getPort() const;

    /// Posa els mitjans disponibles per la impressió (\p mediumType és una llista d'opcions separades per "\").
    void setDefaultMediumType(const QString &mediumType);
    QString getDefaultMediumType() const; 
    void setAvailableMediumTypeValues(QStringList mediumTypeValues);
    QStringList getAvailableMediumTypeValues();

    /// Posa les mides disponibles d'impressió (\p filmSize és una llista d'opcions separades per "\").
    void setDefaultFilmSize(const QString &filmSize);	
    QString getDefaultFilmSize() const;
    void setAvailableFilmSizeValues(QStringList filmSizeValues);
    QStringList getAvailableFilmSizeValues();

    /// Posa els layouts disponibles per la impressió (\p layout és una llista d'opcions separades per "\").
    void setDefaultFilmLayout(const QString &filmLayout);	
    QString getDefaultFilmLayout() const;
    void setAvailableFilmLayoutValues(QStringList filmLayoutValues);	
    QStringList getAvailableFilmLayoutValues();	

    ///Especifica retorna la orientació (horitzontal/vertical) per defecte amb la que s'ha d'imprimir la placa
    void setDefaultFilmOrientation(const QString &filmOrientation);	
    QString getDefaultFilmOrientation() const;
    void setAvailableFilmOrientationValues(QStringList filmOrientationValues);
    QStringList getAvailableFilmOrientationValues();

    /// Posa el Magnification type disponibles per la impressió (\p magnificationType és una llista d'opcions separades per "\").
    void setDefaultMagnificationType(const QString &magnificationType);	
    QString getDefaultMagnificationType() const;
    void setAvailableMagnificationTypeValues(QStringList magnificationTypeValues);
    QStringList getAvailableMagnificationTypeValues();

    ///Especifica/retorna la resolució màxima de la impressió.
    void setDefaultMaxDensity(ushort maxDensity);
    ushort getDefaultMaxDensity() const;
    void setAvailableMaxDensityValues(ushort maxDensityValues);
    ushort getAvailableMaxDensityValues();

    ///Especifica/retorna la resolució mínima de la impressió
    void setDefaultMinDensity(ushort minDensity);
    ushort getDefaultMinDensity() const;
    void setAvailableMinDensityValues(ushort minDensityValues);
    ushort getAvailableMinDensityValues();
    
    /// Posa si es suporta l'ús de TimBox o no.
    void setDefaultTrim(bool trim);
	bool getDefaultTrim() const;
    void setAvailableTrim(bool availableTrim);
    bool getAvailableTrim() const;

    /// Posa les densitats disponibles en el Border  (\p borderDensity és una llista d'opcions separades per "\").
    void setDefaultBorderDensity(const QString &borderDensity);
    QString getDefaultBorderDensity() const;
    void setAvailableBorderDensityValues(QStringList borderDensityValues);
    QStringList getAvailableBorderDensityValues();

    /// Posa les densitats disponibles en la zona buida (EmptyImage) de la impressió  (\p borderDensity és una llista d'opcions separades per "\").
    void setDefaultEmptyImageDensity(const QString &emptyImageDensity);
    QString getDefaultEmptyImageDensity() const;
    void setAvailableEmptyImageDensityValues(QStringList emptyImageDensityValues);
    QStringList getAvailableEmptyImageDensityValues();

    /// Posa els destins disponibles per la impressió  (\p borderDensity és una llista d'opcions separades per "\").
    void setDefaultFilmDestination(const QString &filmDestination);
    QString getDefaultFilmDestination() const;
    void setAvailableFilmDestinationValues(QStringList filmDestinationValues);
    QStringList getAvailableFilmDestinationValues();

    ///Especifica/Retorna el smoothing type per defecte amb la que s'imprimirà
    void setDefaultSmoothingType(const QString &smoothingType);
    QString getDefaultSmoothingType() const;
    void setAvailableSmoothingTypeValues(QStringList smoothingTypeValues);
    QStringList getAvailableSmoothingTypeValues();
    
	///Especifica/retorna la prioritat per defecte dels jobs que s'imprimeixin amb aquesta impressora
    void setDefaultPrintPriority(const QString &priority);
    QString getDefaultPrintPriority() const;
    void setAvailablePrintPriorityValues(QStringList printPriorityValues);
    QStringList getAvailablePrintPriorityValues();
    
    ///Especifica/retorna la polaritat per defecte de l'impressora
    void setDefaultPolarity(const QString &polarity);
    QString getDefaultPolarity() const;
    void setAvailablePolarityValues(QStringList polarityValues);
    QStringList getAvailablePolarityValues();

    void setIsDefault(const bool &isDefault);
    bool getIsDefault() const;


private:		
	
    QString m_aeTitle;
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
};
}; 
#endif
