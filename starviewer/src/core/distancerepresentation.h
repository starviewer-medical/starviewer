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

namespace udg {

class Text;
class Line;

/**
Classe que implementa una especialització de la classe representation, especial per a les distàncies.

@author Grup de Gràfics de Girona  ( GGG )
*/

class DistanceRepresentation : public Representation{
Q_OBJECT
public:
    
    ///constructor per defecte
    DistanceRepresentation();
    
    ///constructor amb paràmetres
    DistanceRepresentation( Line *line, Text *text );
    
    ~DistanceRepresentation();
    
    ///Ens retorna el tipus de representació que és
    QString getRepresentationType()
    { return("DistanceRepresentation"); }
    
    ///assignem un objecte de tipus Text
    void setText( Text *text ); 
    
    ///assignem un objecte de tipus Line
    void setLine( Line *line ); 
    
    ///ens retorna l'atribut de tipus Text
    Text* getText(); 
    
    ///retorna l'atribut de tipus Line
    Line* getLine(); 
    
private:
    
    ///atribut del tipus primitiva de text
    Text *m_text;
    
    ///atribut del tipus primitiva de línia
    Line *m_line;
};

};  //  end  namespace udg

#endif
