/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGROITOOLDATA_H
#define UDGROITOOLDATA_H

#include "tooldata.h"
#include <QList>

//Forward declarations
class QString;

namespace udg {
/**
Classe que implementa una especialització de la classe ToolData, per a contenir dades necessàries per a la Tool de creació de ROIs.

@author Grup de Gràfics de Girona  ( GGG )
*/

class ROIToolData : public ToolData {
Q_OBJECT

private:
    
    typedef QList< double* > PointsList;
    
    ///atribut corresponent als punts de la ROI
    PointsList m_points;
    
    ///atribut que conté el text de la distància
    QString m_text;
    
    ///atribut corresponent a la posició del text de la distància
    double m_textPosition[3];

public:
    
    ///constructor amb paràmetres
    ROIToolData( PointsList points );
    
    ~ROIToolData();
    
    ///assignem els punts del polígon
    void setPoints( PointsList points ); 
    
    ///retornem els punts de la ROI
    PointsList getPoints()
    { return( m_points ); } 

    
    ///retornem el segon punt
    double* getTextPosition()
    { return( m_textPosition ); }
    
    ///retornem el text
    QString getROIText()
    { return( m_text ); }
    
signals:
    
    ///s'emet quan canvia algun punt de la ROI
    void pointsChanged();
    
    ///s'emet quan canvia el text de la ROI
    void roiTextChanged();
};

};  //  end  namespace udg

#endif
