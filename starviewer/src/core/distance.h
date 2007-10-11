/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGDISTANCE_H
#define UDGDISTANCE_H

namespace udg {

/**
Classe que engloba una distància

@author Grup de Gràfics de Girona  ( GGG )
*/
class Distance 
{
public:

    Distance();
    Distance( double* first , double* second );
    ~Distance();
    
    void setFirstPoint( double* first );
    void setSecondPoint( double* second );
    void setPoint( double* point , int n );
    void setPoints( double* first , double* second );

    double* getFirstPoint(){ return m_first; };
    double* getSecondPoint(){ return m_second; };
    
    /// Retorna la distància euclidiana en 2D (només té en compte X i Y)
    double getDistance2D();
     /// Retorna la distància euclidiana en 3D (té en compté X, Y i Z)
    double getDistance3D();
private:
    /// Punts de la distància;
    double m_first[3]; 
    double m_second[3];
};

};  //  end  namespace udg 

#endif
