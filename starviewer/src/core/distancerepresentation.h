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
     
    ///assignem un objecte de tipus DistanceToolData
    void setDistanceToolData( DistanceToolData *dtd );
    
    ///ens retorna l'atribut de tipus Text
    Text* getText(); 
    
    ///retorna l'atribut de tipus Line
    Line* getLine(); 
    
    ///ens retorna l'atribut de tipus DistanceToolData
    DistanceToolData* getDistanceToolData();
    
    ///permet avisar per tal de refrescar la línia
    void refreshLine()
    { m_line->refreshLine(); }
    
    ///permet avisar per tal de refrescar el text
    void refreshText(){}
    
    ///fa que l'atribut DistanceToolData calculi el text de la distància i la posició del mateix
    void calculateTextAndPositionOfDistance();
    
private:
    
    ///atribut del tipus primitiva de text
    Text *m_text;
    
    ///atribut del tipus primitiva de línia
    Line *m_line;
    
    ///atribut del tipus DistanceToolData per tal de guardar dades referents a la distància
    DistanceToolData *m_distanceToolData;

private slots:
    ///actualitza el primer punt de la línia i com a conseqüència també el text i la seva posició
    void updateFirstPointLine();
    
    ///actualitza el segon punt de la línia i com a conseqüència també el text i la seva posició
    void updateSecondPointLine();
};

};  //  end  namespace udg

#endif
