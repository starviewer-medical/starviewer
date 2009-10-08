#ifndef UDGDICOMPRINTPAGE_H
#define UDGDICOMPRINTPAGE_H

#include <QString>

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

    /**Mètode que retorna el número de columnes i files del layout, s'implementa en un sol mètode per no tenir codi duplicat
       si s'implementés un mètode per obtenir el número de columnes i una altre per obtenir el número de files*/
    void getFilmLayoutColumnsRows(int &colums, int &rows);    
    
};
};
#endif