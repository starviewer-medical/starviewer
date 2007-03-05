/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGSHAPEREPRESENTATION_H
#define UDGSHAPEREPRESENTATION_H

#include "shape.h"

class vtkActor2D;
class vtkPolyDataMapper2D;
class vtkCoordinate;

namespace udg {

/**
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
	
	Superclasse de totes les representacions gràfiques 2D possibles que es poden dibuixar sobre un visor. 
        Implementa i conté les operacions i atributs gràfics fonamentals d'aquestes formes.
*/
class ShapeRepresentation{
public:
    ShapeRepresentation();

    ~ShapeRepresentation();
    
    /// ens retorna el tipus d'objecte que és.
    const char* getClassName()
    {return ("ShapeRepresentation");};
    
    ///desactiva la visibilitat de l'actor
    void visibilityOff();
    
    ///activa la visibilitat de l'actor
    void visibilityOn();
    
    ///assigna un color determinat per una cadena de caràcter
    ///Colors que s'accepten:
    ///red, green, light_green, blue, light_blue, yellow, orange, purple, gray, black, white
    void setColor( const char* color );
    
    ///assigna un color definit per 3 reals (manera normal vtk)
    void setColor( double, double, double );
    
    ///establim com a sistema de coordenades les de món
    void setCoordinateSystemToWorld();
    
    ///establim com a sistema de coordenades les de display
    void setCoordinateSystemToDisplay();
    
    ///mètode a implementar per les subclasses
    virtual Shape* getShape(){ return 0; }
    
    ///Ens retorna l'actor vtk 
    vtkActor2D* getActor()
    {return m_shapeActor;}
    
    ///ens retorna el mapper vtk
    vtkPolyDataMapper2D* getMapper()
    {return m_shapeMapper;}
    
protected:
    
    ///actor vtk que encapsula l'objecte a representar
    vtkActor2D                   *m_shapeActor;
            
    ///mapejador de l'actor
    vtkPolyDataMapper2D          *m_shapeMapper;

    ///sistema de coordenades que utilitzem
    vtkCoordinate *m_coordinate;
};

}

#endif
