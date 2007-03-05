/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGPOLYLINE_H
#define UDGPOLYLINE_H

#include <shape.h>
#include <point.h>

//wrapper stl
#include <vector>

namespace udg {

/**
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
	
	Classe que implementa el tipus polilínia,com a subclasse de Shape.
        El tipus polilínia estarà format per una n-tupla de punts que formaran una estructura
        interpolable per un conjunt de línies o per una corba (spline).
*/
class Polyline : public Shape
{
public:
    
    enum { SPLINE , LINES };
    
    Polyline();

    ~Polyline();
    
    /// ens retorna el tipus d'objecte que és.
    const char* getClassName()
    {return ( "Polyline" );};
    
    ///permet inserir un punt de la polilínia
    void addPoint( Point point );
    
    ///ens permet esborrat l'últim punt
    void removeLastPoint();
    
    ///ens permet esborrar el primer punt
    void removeFirstPoint();

    /// ens permet esborrar el punt situat a la posició index
    void removePointOfPosition( unsigned int index );
    
    ///ens permet obtenir el primer punt
    Point getFirstPoint();
    
    ///ens permet obtenir l'últim punt
    Point getLastPoint();
    
    ///ens permet obtenir el punt de la posició index
    Point getPointOfPosition( unsigned int index );
    
    ///ens permet determinar el tipus d'interpolació que volem
    void setInterpolationType( int interpolation )
    { m_typeOfInterpolation = interpolation; }
    
    ///ens retorna el tipus d'interpolació que estem utilitzant
    int getInterpolationType()
    { return ( m_typeOfInterpolation ); }
    
    ///ens retorna la longitud de punts que conté la polilínia
    int getNumberOfPoints();
    
    ///col·loquem l'iterador a l'inici de l'estructura
    void goToBegin();
    
    ///col·loquem l'iterador al final de l'estructura
    void goToEnd();
    
    ///mou l'iterador a la següent posició
    void next();
    
    ///mou l'iterador a la posició anterior
    void previous();
    
    /// retorna el punt corresponent a la posició on es troba actualment l'iterador
    Point getCurrentPoint();
    
    ///ens diu si hi ha un següent element
    bool hasNext();
    
    ///ens diu si hi ha un element anterior a l'actual
    bool hasPrevious();
    
    /** Càlcul de distàncies
        Si la polilínia té més de dos punts, la distància resultant és la suma de les sub-distàncies 
        entre els diferents parells de punts. 
    */
    
    /// Retorna la distància euclidiana en 2D (només té en compte X i Y)
    double getDistance2D();
     /// Retorna la distància euclidiana en 3D (té en compté X, Y i Z)
    double getDistance3D();
    
private:
    
    /// conté el tipus d'interpolació que desitgem fer sobre la polilínia.
    int m_typeOfInterpolation;    
        
    /// estructura de dades del tipus vector per guardar els punts que formen la polylínia
    std::vector <Point> m_polylinePoints;
    std::vector <Point>::iterator it;
    
};

}

#endif
