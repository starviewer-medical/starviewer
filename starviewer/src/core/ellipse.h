/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGELLIPSE_H
#define UDGELLIPSE_H

#include "roi.h"
#include "point.h"

namespace udg {

/**
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
	
	Classe que implementa el tipus ellipse,com a subclasse de Shape i en particular de ROI.
        El tipus Ellipse té com a atributs 4 punts que pertanyen a l'eix major i a l'eix menor.
*/
class Ellipse : public ROI
{
public:
    enum { ELLIPSE , CIRCLE };
    
    Ellipse();

    ~Ellipse();
    
    /// ens retorna el tipus d'objecte que és.
    const char* getClassName()
    {return ( "Ellipse" );};
    
    ///ens permet determinar el comportament de l'el·lipse
    void setBehavior( int behavior )
    { m_behavior = behavior; };
    
    ///ens retorna el comportament de l'el·lipse
    int getBehavior()
    { return ( m_behavior ); };
    
    ///ens permet assignar el radi menor de l'el·lipse
    void setMinorRadius( Point );
    
    ///ens permet assignar el radi major de l'el·lipse
    void setMajorRadius( Point );
    
    ///ens permet assignar el centre de l'el·lipse
    void setCenter( Point );
    
    ///retorna el centre de l'el·lipse
    Point getCenter();
    
    ///retorna el radi menor de l'el·lipse
    Point getMinorRadius();
    
    ///retorna el radi major de l'el·lipse
    Point getMajorRadius();
    
    ///converteix l'el·lipse en cercle: simplement fa que els eixos major i menor concideixin,
    ///donant a l'eix menor els valors de l'eix major i determina el comportament com a cercle
    void convertToCircle();
    
    ///Calcula l'àrea de l'el·lipse (del cercle si s'està comportant com a tal)
    void computeArea();
    
private:
    
        Point m_minorRadius;
        Point m_majorRadius;
        Point m_center;
        
        
        ///ens determina el comportament de l'el·lipse: com a el·lipse o com a cercle (cas particular 
        ///d'una el·lipse).
        int m_behavior;
};

}

#endif
