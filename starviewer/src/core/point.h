/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGPOINTPRIMITIVE_H
#define UDGPOINTPRIMITIVE_H

#include "drawingprimitive.h"

namespace udg {

/**

Classe que implementa la primitiva gràfica de tipus Punt. Hereta de la classe DrawPrimitive.

@author Grup de Gràfics de Girona  ( GGG )
*/

class Point : public DrawingPrimitive{
Q_OBJECT
public:
    
    /**propietats especials que aporten els punts:
        -punt arrodonit o quadrat
        -interior ple o buit
     */
    
    ///constructor per defecte
    Point();
    
    ///constructor amb paràmetres: passem la posició del punt 
    Point( double position[3] );
    
    ~Point();
    
    ///permet canviar la posició del punt 
    void setPosition( double point[3] );
    
    ///retorna la posició del punt
    double* getPosition()
    { return( m_position ); }
    
    ///Determinem que el punt es dibuixi en forma arrodonida
    void rounded()
    { m_rounded = true; }
    
    ///Determinem que el punt es dibuixi en forma quadrada
    void squared()
    { m_rounded = false; }
    
    bool isRounded()
    { return( m_rounded ); }
    
    ///Determinem que el punt es dibuixi ple
    void filledOn()
    { m_filled = true; }
    
    ///Determinem que el punt es dibuixi buit
    void filledOff()
    { m_filled = false; }
    
    void filled( bool filled )
    { m_filled = filled; }
    
    bool isFilled()
    { return( m_filled ); }
    
private:
    
    ///posició del punt
    double m_position[3];
    
    ///Ens determina si el punt es dibuixarà arrodonit o no
    bool m_rounded;
    
    ///determina si el punt és ple o no: si no és ple tindrà forma de disc
    bool m_filled;

};

};  //  end  namespace udg

#endif
