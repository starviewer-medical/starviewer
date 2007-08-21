/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGDISTANCETOOLDATA_H
#define UDGDISTANCETOOLDATA_H

#include "tooldata.h"


namespace udg {

/**
Classe que implementa una especialització de la classe ToolData, per a contenir dades necessàries per a la Tool de creació de distàncies.

@author Grup de Gràfics de Girona  ( GGG )
*/

class DistanceToolData : public ToolData{
Q_OBJECT
public:
    
    ///constructor amb paràmetres
    DistanceToolData( double p1[3], double p2[3] );
    
    ~DistanceToolData(){}
    
    ///assignem el primer punt
    void setFirstPoint( double p1[3] ); 
    
    ///assignem el segon punt
    void setSecondPoint( double p2[3] );
    
    ///retornem el primer punt
    double* getFirstPoint()
    { return( m_firstPoint ); } 
    
    ///retornem el segon punt
    double* getSecondPoint()
    { return( m_secondPoint ); }
    
private:
    
    ///atributs corresponents als punts de la distància
    double m_firstPoint[3];
    double m_secondPoint[3];
};

};  //  end  namespace udg

#endif
