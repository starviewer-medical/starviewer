/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGELLIPSE_H
#define UDGELLIPSE_H

#include "polygon.h"
#include <QString>

namespace udg {

/**
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
	
	Classe que implementa la primitiva gràfica El·lipse. També engloba el cas del cercle, ja que podem tractar el cercle com a cas especial d'una el·lipse, si fem que coincideixin els dos centres d'aquesta. Hereta de la classe Polygon, pertant també és subclasse de DrawPrimitive.
*/
class Ellipse : public Polygon
{
public:
    
    /**propietats especials que aporten les el·lipses:
        -radi major
        -radi menor
        -centre
        -comportament
    
        -NOTA: normalment una el·lipse es tracta amb eix major/menor i no radi major/menor, ho fem així per facilitar la feina a l'hora de trobar els punts que formen l'el·lipse. Quan parlem del radi major, per exemple, ens referim al segment que va des del centre fins  a un dels extrems de l'eix major, anàlogament per l'eix menor.  
     */
    
    ///Constructor amb paràmetres: passant el centre i els dos radis d'una el·lipse
    Ellipse( double minorRadius[3], double majorRadius[3], double center[3], QString behavior );

    ///Constructor amb paràmetres: passant els extrems superior esquerre i inferior dret del rectangle en el qual s'inscriu l'el·lipse
    Ellipse( double topLeft[3], double bottomRight[3], QString behavior );
    
    ~Ellipse();
    
    ///ens permet determinar el comportament de l'el·lipse
    void setBehavior( QString behavior )
    { m_behavior = behavior; };
    
    ///ens retorna el comportament de l'el·lipse
    QString getBehavior()
    { return ( m_behavior ); };
    
    ///ens permet assignar el radi menor de l'el·lipse
    void setMinorRadius( double point[3] );
    
    ///ens permet assignar el radi major de l'el·lipse
    void setMajorRadius( double point[3] );
    
    ///ens permet assignar el centre de l'el·lipse
    void setCenter( double point[3] );
    
    ///retorna el centre de l'el·lipse
    double* getCenter()
    { return( m_center ); }
    
    ///retorna el radi menor de l'el·lipse
    double* getMinorRadius()
    { return( m_minorRadius ); }
    
    ///retorna el radi major de l'el·lipse
    double* getMajorRadius()
    { return( m_majorRadius ); }
    
    ///ens permet assignar el punt superior esquerre del rectangle en el qual s'incriu l'el·lipse
    void setTopLeftPoint( double point[3] );
    
    ///ens permet assignar el punt inferior dret del rectangle en el qual s'incriu l'el·lipse
    void setBottomRightPoint( double point[3] );
    
    ///ens retorna el punt superior esquerre del rectangle en el qual s'incriu l'el·lipse
    double* getTopLeftPoint()
    { return( m_topLeft ); }
    
    ///ens retorna el punt inferior dret del rectangle en el qual s'incriu l'el·lipse
    double* getBottomRightPoint()
    { return( m_bottomRight ); }
    
    ///converteix l'el·lipse en cercle: simplement fa que els eixos major i menor concideixin,
    ///donant a l'eix menor els valors de l'eix major i determina el comportament com a cercle
    ///Cal tenir en compte que s'agafa el valor de l'EIX MAJOR.
    void convertToCircle();
    
    QString getPrimitiveType()
    { return( "Ellipse" ); }
        
private:
    ///calcula els punts del rectangle que inscriu una el·lipse a partir del centre i els radis
    void computeRectanglePoints();
    
    ///calcula el centre i els radis d'una el·lipse apartir dels punts del rectangle que la inscriu 
    void computeCenterAndRadius();
    
private:
        ///Eixos i centre
        double m_minorRadius[3];
        double m_majorRadius[3];
        double m_center[3];
        
        ///punts del requadre que inscriu l'el·lipse
        double m_topLeft[3];
        double m_bottomRight[3];
        
        ///ens determina el comportament de l'el·lipse: com a el·lipse o com a cercle (cas particular 
        ///d'una el·lipse).
        QString m_behavior;
};

}

#endif
