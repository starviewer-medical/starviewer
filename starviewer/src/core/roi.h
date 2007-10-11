/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGROI_H
#define UDGROI_H

#include <shape.h>

namespace udg {

/**
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
	
	Classe que implementa la classe base ROI,com a subclasse de Shape i de la qual derivaran totes 
        les formes que es poden usar com a roi, és a dir, regions tancades d'interés.
*/

class ROI : public Shape
{
public:
    ROI();

    ~ROI();

    /// ens retorna el tipus d'objecte que és.
    const char* getClassName()
    {return ( "ROI" );};
    
    ///Calcula l'àrea de la roi (mètode a implementar per a cada subclassse de roi)
    virtual void computeArea(){}
    
    double getArea()
    {return m_area;};
    
protected:
    
    double m_area;
    
};

}

#endif
