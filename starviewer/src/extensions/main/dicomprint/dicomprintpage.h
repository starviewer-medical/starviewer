#ifndef UDGDICOMPRINTPAGE_H
#define UDGDICOMPRINTPAGE_H

#include <QString>
#include <QMap>

#include "printpage.h"

/**
* Implementació per la Interfície DicomPrintPage que defineix una pàgina amb imatges per imprimir en impressores Dicom en DCMTK.
*/

namespace udg
{

class Image;

class DicomPrintPage: public PrintPage
{
public:
    
    ///Especifica/Retorna la mida sobre de la placa sobre el que s'imprimirà
    void setFilmSize(const QString &filmSize);	
    QString getFilmSize() const;

    ///Especifica/Retorna layout de la placa sobre el que s'imprimirà
    void setFilmLayout(const QString &filmLayout);	
    QString getFilmLayout() const;

    ///Retorna el número de columnes/files del layout
    int getFilmLayoutColumns();
    int getFilmLayoutRows();
       
    ///Especifica/Retorna l'orientació (Horitzontal/Vertical) de la placa en que s'imprimirà
    void setFilmOrientation(const QString &filmOrientation);	
    QString getFilmOrientation() const;

    ///Especifica/retorna el Magnification amb le que s'imprimirà
    void setMagnificationType(const QString &magnificationType);	
    QString	getMagnificationType() const;

    ///Especifica/retorna la densitat màxima amb la que s'imprimirà
    void setMaxDensity(ushort maxDensity);
    ushort getMaxDensity();

    ///Especifica/retorna la densitat mínima amb la que s'imprimirà
    void setMinDensity(ushort minDensity);
    ushort getMinDensity();
 
    ///Especifica si s'imprimirà les imatges amb trim o sense
    void setTrim(bool trim);
    bool getTrim();

    ///Especifica/retorna la densitat amb la que s'imprimirà la línia de separació entre les imatges
    void setBorderDensity(const QString &borderDensity);
    QString getBorderDensity() const;

    ///Especifica/retorna la densitat amb que s'imprimiran els image box sense imatge
    void setEmptyImageDensity(const QString &emptyImageDensity);
    QString getEmptyImageDensity() const;

    ///Especifica/Retorna el smoothing type per defecte amb la que s'imprimirà
    void setSmoothingType(const QString &smoothingType);
    QString getSmoothingType() const;

    ///Especifica/retorna la polaritat per defecte que s'utilitzarà per imprimir
    //Aquest paràmetre a nivell dicom s'aplica per imatge no per pàgina
    void setPolarity(const QString &polarity);
    QString getPolarity() const;

    ///Especifica retorna la informació del tag Configuration Information
    void setConfigurationInformation(const QString &configurationInformation);
    QString getConfigurationInformation() const;

    ///Ens afegeix una anotació a la pàgina en la posició indicada. El text de l'anotació no pot fer més de 64 caràcters, si fa més de 64 caràcters el trunca
    ///Si s'afegeix una anotació en una posició que ja hi havia una altra notació aquesta se sobre escriu, i si el text és buit no afegeix l'anotació
    ///La majoria d'impressores només tenen 6 posicions per les anotacions Agfa, Kodak, Sony, Codonics. Tenir en compte que totes les impressores les posicions
    ///de les anotacions són de la 1 a la 6 i Sony és de la 0 a la 5.
    void addAnnotation(int position, const QString &annotationText);
    
    ///Retorna les anotacions que ens han indicat que s'han d'imprimir amb la pàgina
    QMap<int, QString> getPageAnnotations();



private:

    QString m_filmSize;
    QString	m_filmLayout;
    QString m_filmOrientation;
    QString	m_magnificationType;
    bool m_trim;
    QString m_borderDensity;
    QString m_emptyImageDensity;
    QString m_smoothingType;
    QString m_polarity;
    ushort m_minDensity, m_maxDensity;
    QString m_configurationInformation;
    QMap<int, QString> m_pageAnnotations;
    QString m_annotationDisplayFormatID;
};
};
#endif