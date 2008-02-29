/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "distance.h"
#include <math.h>


namespace udg {

Distance::Distance()
{
}

Distance::Distance( double* first , double* second )
{
    setPoints( first , second );
}

Distance::~Distance()
{
}

void Distance::setPoint( double* point , int n )
{
    if( n <= 1 )
        setFirstPoint( point );
    else
        setSecondPoint( point );
}
    
void Distance::setPoints( double* first , double* second )
{
    setFirstPoint( first );
    setSecondPoint( second );
}

void Distance::setFirstPoint( double* first )
{ 
    for ( int i = 0; i < 3; i++ )
        m_first[i] = first[i]; 
}

void Distance::setSecondPoint( double* second )
{ 
    for ( int i = 0; i < 3; i++ )
        m_second[i] = second[i]; 
}

double Distance::getDistance2D()
{
    double xx = (m_first[0] - m_second[0]);
    double yy = (m_first[1] - m_second[1]);
    double valor = pow(xx, 2) + pow(yy, 2);
    return (sqrt(valor));
}

double Distance::getDistance3D()
{
    double xx = (m_first[0] - m_second[0]);
    double yy = (m_first[1] - m_second[1]);
    double zz = (m_first[2] - m_second[2]);
    double valor = pow(xx, 2) + pow(yy, 2) + pow(zz, 2);
    return (sqrt(valor));
}

};  // end namespace udg 
