/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGELLIPSEREPRESENTATION_H
#define UDGELLIPSEREPRESENTATION_H

#include "representation.h"
#include <QString>
#include "ellipse.h"
#include "text.h"

namespace udg {

class ROIToolData;

/**
Classe que implementa una especialització de la classe representation, especial per a les el·lipses.

@author Grup de Gràfics de Girona  ( GGG )
*/

class EllipseRepresentation : public Representation{
Q_OBJECT
public:
    
    ///constructor amb paràmetres
    EllipseRepresentation( ROIToolData *rtd );
    
    ~EllipseRepresentation();
    
    ///Ens retorna el tipus de representació que és
    QString getRepresentationType()
    { return("EllipseRepresentation"); }
    
    ///assignem un objecte de tipus Text
    void setText( Text *text ); 

    ///assignem el el·lipse
    void setEllipse( Ellipse *ellipse );
     
    ///assignem un objecte de tipus ROIToolData
    void setROIToolData( ROIToolData *rtd );
    
    ///ens retorna l'atribut de tipus Text
    Text* getText(); 

    ///retornem l'atribut de tipus el·lipse
    Ellipse* getEllipse();
    
    ///ens retorna l'atribut de tipus ROIToolData
    ROIToolData* getROIToolData();
    
    ///permet avisar per tal de refrescar el text
    void refreshText( double *topLeft, double *bottomRight, int view );
    
    ///permet avisar per tal de refrescar l'el·lipse
    void refreshEllipse();
    
    ///fa que l'atribut ROIToolData calculi el text de la distància i la posició del mateix
    void calculateTextAndPositionOfEllipse( int view );
    
private:
    
    ///atribut del tipus primitiva de text
    Text *m_text;

    ///atribut que representa el voltant del text de la roi
    Ellipse *m_ellipse;
    
    ///atribut del tipus ROIToolData per tal de guardar dades referents a la roi. De la ROIToolData, el primer punt es correspondrà amb el superior esquerre i el segon es correspondrà amb l'inferior dret de l'el·lipse
    ROIToolData *m_roiToolData;

private slots:
    
    ///actualitza el text de la roi. L'actualització del text de la roi no es realitza cada cop que canvia un punt, ja que és innecessari perquè podem estar movent un punt i mentre s'està editant no volem que es vegi el text. El text es mostra en el moment en que la roi ha estat fixada.
    void updateText();
    
    ///actualitza els punts de l'el·lipse
    void updateEllipsePoints();
};

};  //  end  namespace udg

#endif
