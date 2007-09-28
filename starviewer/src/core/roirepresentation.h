/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGROIREPRESENTATION_H
#define UDGROIREPRESENTATION_H

#include "representation.h"
#include <QString>
#include "polygon.h"
#include "text.h"

namespace udg {

class ROIToolData;

/**
Classe que implementa una especialització de la classe representation, especial per a les ROIs.

@author Grup de Gràfics de Girona  ( GGG )
*/

class ROIRepresentation : public Representation{
Q_OBJECT
public:
    
    ///constructor amb paràmetres
    ROIRepresentation( ROIToolData *dtd );
    
    ~ROIRepresentation();
    
    ///Ens retorna el tipus de representació que és
    QString getRepresentationType()
    { return("ROIRepresentation"); }
    
    ///assignem un objecte de tipus Text
    void setText( Text *text ); 

    ///assignem el polígon
    void setPolygon( Polygon *polygon );
     
    ///assignem un objecte de tipus ROIToolData
    void setROIToolData( ROIToolData *dtd );
    
    ///ens retorna l'atribut de tipus Text
    Text* getText(); 

    ///retornem l'atribut de tipus polígon
    Polygon* getPolygon();
    
    ///ens retorna l'atribut de tipus ROIToolData
    ROIToolData* getROIToolData();
    
    ///permet avisar per tal de refrescar el text i el voltant del text
    void refreshText( int view );
    
    ///permet avisar per tal de refrescar el polígon
    void refreshPolygon()
    { m_polygon->refreshPolygon(); }
    
    ///fa que l'atribut ROIToolData calculi el text de la distància i la posició del mateix
    void calculateTextAndPositionOfROI( int view );
    
private:
    
    ///atribut del tipus primitiva de text
    Text *m_text;

    ///atribut que representa el voltant del text de la roi
    Polygon *m_polygon;
    
    ///atribut del tipus ROIToolData per tal de guardar dades referents a la roi
    ROIToolData *m_roiToolData;

private slots:
    
    ///actualitza el text de la roi. L'actualització del text de la roi no es realitza cada cop que canvia un punt, ja que és innecessari perquè podem estar movent un punt i mentre s'està editant no volem que es vegi el text. El text es mostra en el moment en que la roi ha estat fixada.
    void updateText();
    
    ///actualitza els punts del polígon
    void updatePolygonPoints();
};

};  //  end  namespace udg

#endif
