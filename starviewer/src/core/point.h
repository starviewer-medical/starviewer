/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGPOINT_H
#define UDGPOINT_H

#include "shape.h"

namespace udg {

/**
@author Grup de Gràfics de Girona  ( GGG )

Classe que implementa un punt geomètric en 2D,com a subclasse de Shape.
*/
class Point   : public Shape
{
public:
    Point();
    Point( double points[2] );
    Point( double x , double y );
    ~Point();

    void setX( double x ){ m_point[0] = x; }
    void setY( double y ){ m_point[1] = y; }
    void setValues( double x , double y );
    void setValues( double points[2] );
    
    double getX(){ return m_point[0]; }
    double getY(){ return m_point[1]; }
    
    double* getPoint();

    /// ens retorna el tipus d'objecte que és.
    const char* getClassName()
    {return ( "Point" );}
    
private:
    /// Coordenades
    double m_point[2];
};

};  //  end  namespace udg 

#endif
