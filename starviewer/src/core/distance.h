/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGDISTANCE_H
#define UDGDISTANCE_H

#include "point.h"

namespace udg {

/**
Classe que engloba una distància

@author Grup de Gràfics de Girona  ( GGG )
*/
class Distance 
{
public:

    Distance();
    Distance( Point first , Point second );
    Distance( Point points[2] );
    ~Distance();
    
    void setFirstPoint( Point first ){ m_first = first; };
    void setSecondPoint( Point second ){ m_second = second; };
    void setPoint( Point point , int n );
    void setPoints( Point first , Point second );

    Point getFirstPoint(){ return m_first; };
    Point getSecondPoint(){ return m_second; };
    void getPoints( Point points[2] ){ points[0] = m_first; points[2] = m_second; };

    /// Retorna la distància euclidiana
    double getDistance();
private:
    /// Coordenades geomètriques de la mesura;
    Point m_first, m_second;
};

};  //  end  namespace udg 

#endif
