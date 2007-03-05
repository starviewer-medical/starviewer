/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGELLIPSEREPRESENTATION_H
#define UDGELLIPSEREPRESENTATION_H

#include <shaperepresentation.h>
#include <ellipse.h>

class vtkDiskSource;

namespace udg {

    class Point;
/**
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
    
    Representació gràfica 2D d'una el·lipse (també cercle) sobre un visor. 
    Implementa i conté les operacions i atributs gràfics fonamentals d'aquesta forma.
*/
    
class EllipseRepresentation : public ShapeRepresentation
{
public:
    EllipseRepresentation();
    
        ///Constructor amb paràmetres: passem com a paràmetres els dos radis del punt i la resolució
//     EllipseRepresentation( double inner , double outer , int resolution, double position[2]);

    ~EllipseRepresentation();
    
    ///defineix el radi petit de l'el·lipse.
    void setMinorRadius( Point );
    
    void setMinorRadius( double pos[2] );
    
    ///defineix el radi gran de l'el·lipse.
    void setMajorRadius ( Point );
    
    void setMajorRadius ( double pos[2] );
    
    /// defineix el centre de l'el·lipse.
    void setCenter( double pos[2] );
    void setCenter( Point );
    
    /// retorna l'atribut del tipus shape (subclasse de Shape)
    Shape* getShape()
    {return m_ellipseShape;};
    
    private:
        
//POSAR UN ARRAY DE PUNTS QUE SERÀN TOTS ELS PUNTS DE LELIPSE
       
    ///Punt que encapsula aquesta representació
        Ellipse* m_ellipseShape;

};

}

#endif