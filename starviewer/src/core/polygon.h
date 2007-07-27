/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGPOLYGON_H
#define UDGPOLYGON_H

#include "drawingprimitive.h"
#include <QList>

namespace udg {

/**

Classe que implementa la primitiva gràfica polígon. Hereta de la classe DrawPrimitive.

@author Grup de Gràfics de Girona  ( GGG )
*/

class Polygon : public DrawingPrimitive{
Q_OBJECT
public:
    
    /**propietats especials que aporten els polígons:
        -Background de les primitives polígon
        -Background, per defecte, de les primitives polígon
     */
    
    ///constructor amb paràmetres: passem els punts
    Polygon( QList< double* > points );
    
    ~Polygon();
    
    ///ens permet assignar el color del fons de les primitives polígon
    void setBackgroundColor( QColor color )
    { m_primitiveBackgroundColor = color; }
    
    ///ens retorna el color del fons d'una determinada primitiva polígon
    QColor getBackgroundColor() const
    { return m_primitiveBackgroundColor; }
    
    ///ens retorna el color per defecte del fons dels polígons
    QColor getDefaultBackgroundColor() const
    { return DefaultBackgroundColor; }
    
     ///determinem que s'empleni el fons de la primitiva 
    void enableBackground()
    {  background( true ); }
    
    ///determinem que no s'empleni el fons de la primitiva 
    void disableBackground()
    { background( false ); }
    
    ///permet determinar si el fons de la primitiva està ple o no mitjançant un paràmetre
    void background( bool enabled )
    { m_backgroundEnabled = enabled; }   
     
    void setPoints( QList< double* > points )
    { m_pointsList = points; }
    
    QList<double*> getPoints() const
    { return( m_pointsList ); }
    
    int getNumberOfPoints();
    
private:
    
    ///Background de les primitives polígon.
    QColor m_primitiveBackgroundColor;
    
    ///Background, per defecte, dels polígons.
    QColor DefaultBackgroundColor;
    
    ///conjunt de punts que formaran el polígon
    QList< double* > m_pointsList;

};

};  //  end  namespace udg

#endif
