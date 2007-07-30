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
        -continuitat / discontinuitat de la línia
     */
    
    ///constructor amb paràmetres: passem els punts
    Polygon( QList< double* > points );
    
    ~Polygon();
    
     ///determinem que s'empleni el fons de la primitiva 
    void enableBackground()
    {  background( true ); }
    
    ///determinem que no s'empleni el fons de la primitiva 
    void disableBackground()
    { background( false ); }
    
    ///retorna si té background o no
    bool isBackgroundEnabled()
    { return( m_backgroundEnabled ); }
    
    ///permet determinar si el fons de la primitiva està ple o no mitjançant un paràmetre
    void background( bool enabled )
    { m_backgroundEnabled = enabled; }   
     
    ///assignem la llista de punts que formen el polígon
    void setPoints( QList< double* > points )
    { m_pointsList = points; }
    
    ///ens retorna la llista de punts que formen el polígon
    QList<double*> getPoints() const
    { return( m_pointsList ); }
    
    ///ens diu el nombre de punts que té el polígon
    int getNumberOfPoints();
    
    ///determinem que la primitiva es dibuixi amb línia discontínua
    void discontinuousOn()
    { discontinuous( true ); }
    
    ///determinem que la primitiva es dibuixi amb línia contínua
    void discontinuousOff()
    { discontinuous( false ); }
    
    ///permet determinar la continuïtat de la primitiva mitjançant un paràmetre
    void discontinuous( bool discont )
    { m_discontinuousPrimitive = discont; }
    
    ///ens retorna el booleà que ens diu si hem determinat com a continu o discontinu
    bool isDiscontinuous()
    { return m_discontinuousPrimitive; }
    
private:
    
    ///conjunt de punts que formaran el polígon
    QList< double* > m_pointsList;
    
    ///determina si la primitiva es dibuixa discontínua o no.
    bool m_discontinuousPrimitive;
    
    ///ens determina si el fons de les primitives polígon
    bool m_backgroundEnabled;

};

};  //  end  namespace udg

#endif
