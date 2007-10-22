/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGLINE_H
#define UDGLINE_H

#include "drawingprimitive.h"

namespace udg {

/**

Classe que implementa la primitiva gràfica de tipus línia. Hereta de la classe DrawPrimitive.

@author Grup de Gràfics de Girona  ( GGG )
*/

class Line : public DrawingPrimitive{
Q_OBJECT
public:
    
    /**propietats especials que aporten les línies:
        -continuitat / discontinuitat de la línia
     */
    
    ///constructor per defecte
    Line();
    
    ///constructor amb paràmetres: passem els dos extrems de la línia
    Line( double point1[3], double point2[3] );
    
    ~Line();
    
    ///permet canviar el primer punt de la línia
    void setFirstPoint( double point[3] );
    
    ///retorna el primer punt de la línia
    double* getFirstPoint()
    { return( m_firstPoint ); }
    
    ///permet canviar el segon punt de la línia
    void setSecondPoint( double point[3] );
    
    ///retorna el segon punt de la línia
    double* getSecondPoint()
    { return( m_secondPoint ); }
    
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
    
    ///ens retorna el tipus de primitiva que és
    QString getPrimitiveType()
    { return( "Line" ); }
    
    ///ens emet el senyal per que es refresqui la línia
    void refreshLine();
    
signals:
    ///s'emet quan canvia un dels atributs de la línia
    void lineChanged( Line *line );
    
private:
    
    ///primer punt de la línia
    double m_firstPoint[3];
    
    ///segon punt de la línia
    double m_secondPoint[3];
    
    ///determina si la primitiva es dibuixa discontínua o no.
    bool m_discontinuousPrimitive;
    
};

};  //  end  namespace udg

#endif
