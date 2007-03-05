/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGPOINTREPRESENTATION_H
#define UDGPOINTREPRESENTATION_H

#include <shaperepresentation.h>
#include "point.h"

class vtkDiskSource;

namespace udg {


    
/**
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
	
	Representació gràfica 2D d'un punt sobre un visor. 
        Implementa i conté les operacions i atributs gràfics fonamentals d'aquesta forma.
*/
class PointRepresentation : public ShapeRepresentation
{
public:
    ///Constructor per defecte: dóna valors per defecte als atributs del punt
    PointRepresentation();
    
    ///Constructor amb paràmetres: passem com a paràmetres els dos radis del punt i la resolució
    PointRepresentation( double inner , double outer , int resolution, double position[2]);

    ~PointRepresentation();
    
    ///defineix el radi interior del punt: si volem un punt totalment tancat, aquest radi ha de ser 0.
    void setInnerRadius( double );
    
    ///defineix el radi exterior del punt
    void setOuterRadius ( double );
    
    ///defineix la resolució del punt: a més resolució més circular serà la seva forma.
    void setCircumferentialResolution ( int );
    
    /// defineix la posició del punt
    void setPosition( double pos[2] );
    
    /// retorna l'atribut del tipus shape (subclasse de Shape)
    Shape* getShape()
    {return m_pointShape;};
    
private:
    ///Encapsula l'objecte punt
    vtkDiskSource *m_point; 
       
    ///Punt que encapsula aquesta representació
    Point* m_pointShape;
};

}

#endif
