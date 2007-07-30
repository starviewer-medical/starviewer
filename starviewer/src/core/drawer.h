/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGDRAWER_H
#define UDGDRAWER_H

#include <QObject>

//Foreward declarations
class vtkCoordinate;

namespace udg {

//Forward declarations
class Point;
class Line;
class Text;
class Polygon;
class Q2DViewer;
class DrawingPrimitive;

/**
Classe experta en dibuixar totes les primitives gràfiques que podem representar en un visualitzador

@author Grup de Gràfics de Girona  ( GGG )
*/

class Drawer : public QObject{
    Q_OBJECT
public:
    
    Drawer( Q2DViewer *m_viewer , QObject *parent = 0 );
    ~Drawer();
    
    ///dibuixa un punt amb els atributs passats dins l'objecte passat per paràmetre
    void drawPoint( Point *point);
    
    ///dibuixa una línia amb els atributs passats dins l'objecte passat per paràmetre
    void drawLine( Line *line );
    
    ///dibuixa text amb els atributs passats dins l'objecte passat per paràmetre
    void drawText( Text *text );
    
    ///dibuixa el polígon passat per paràmetre
    void drawPolygon( Polygon *polygon );
private:
    ///Retorna el sistema de coordenades segons l'especificat per paràmetre 
    vtkCoordinate *getCoordinateSystem( QString coordinateSystem );
    
    ///permet assignar un determinat sistema de coordenades a un objecte vtkCoordinate.
    void setCoordinateSystem( QString coordinateSystem, vtkCoordinate *coordinates );
    
private:
    Q2DViewer *m_2DViewer;
};

};  
#endif

