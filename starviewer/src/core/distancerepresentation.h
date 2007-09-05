/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGDISTANCEREPRESENTATION_H
#define UDGDISTANCEREPRESENTATION_H

#include "representation.h"
#include <QString>
#include "line.h"
#include "text.h"
#include "polygon.h"

namespace udg {

class DistanceToolData;

/**
Classe que implementa una especialització de la classe representation, especial per a les distàncies.

@author Grup de Gràfics de Girona  ( GGG )
*/

class DistanceRepresentation : public Representation{
Q_OBJECT
public:
    
    ///constructor amb paràmetres
    DistanceRepresentation( DistanceToolData *dtd );
    
    ~DistanceRepresentation();
    
    ///Ens retorna el tipus de representació que és
    QString getRepresentationType()
    { return("DistanceRepresentation"); }
    
    ///assignem un objecte de tipus Text
    void setText( Text *text ); 
    
    ///assignem un objecte de tipus Line
    void setLine( Line *line );

    ///assignem el polígon
    void setPolygon( Polygon *polygon );
     
    ///assignem un objecte de tipus DistanceToolData
    void setDistanceToolData( DistanceToolData *dtd );
    
    ///ens retorna l'atribut de tipus Text
    Text* getText(); 
    
    ///retorna l'atribut de tipus Line
    Line* getLine(); 

    ///retornem l'atribut de tipus polígon
    Polygon* getPolygon();
    
    ///ens retorna l'atribut de tipus DistanceToolData
    DistanceToolData* getDistanceToolData();
    
    ///permet avisar per tal de refrescar la línia
    void refreshLine()
    { m_line->refreshLine(); }
    
    ///permet avisar per tal de refrescar el text i el voltant del text
    void refreshText( int view );
    
    ///permet avisar per tal de refrescar el polígon
    void refreshPolygon()
    { m_polygon->refreshPolygon(); }
    
    ///fa que l'atribut DistanceToolData calculi el text de la distància i la posició del mateix
    void calculateTextAndPositionOfDistance( int view );
    
private:
    
    ///atribut del tipus primitiva de text
    Text *m_text;
    
    ///atribut del tipus primitiva de línia
    Line *m_line;

    ///atribut que representa el voltant del text de la distància
    Polygon *m_polygon;
    
    ///atribut del tipus DistanceToolData per tal de guardar dades referents a la distància
    DistanceToolData *m_distanceToolData;

private slots:
    ///actualitza el primer punt de la línia 
    void updateFirstPointLine();
    
    ///actualitza el segon punt de la línia
    void updateSecondPointLine();
    
    ///actualitza el text de la distància. L'actualització del text de la distància no es realitza cada cop que canvia un punt, ja que és innecessari perquè podem estar movent un punt de la distància i mentre s'està editant no volem que es vegi el text. El text es mostra en el moment en que la distància ha estat fixada.
    void updateText();
};

};  //  end  namespace udg

#endif
