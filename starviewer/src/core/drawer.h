/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGDRAWER_H
#define UDGDRAWER_H

#include <QObject>
#include <QString>

//Foreward declarations
class vtkCoordinate;
class QColor;
class QString;

namespace udg {

//Forward declarations
class Point;
class Line;
class Text;
class Polygon;
class Ellipse;
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
    
    ///dibuixa l'el·lipse passada per paràmetre
    void drawEllipse( Ellipse *ellipse );
    
    ///els editors gràfics solen enmarcar les el·lipses dins d'un rectangle, sense tenir en compte els eixos major i menor i el centre.
    ///Aquest mètode serveix per fer aquesta operació: passem com a punts els marges superior esquerre i l'inferior dret del rectangle que conté l'el·lipse.
    void drawEllipse( double rectangleCoordinate1[3], double rectangleCoordinate2[3], QColor color, QString behavior );
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

