/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGSHAPETOOL_H
#define UDGSHAPETOOL_H

#include <tool.h>
#include <QMultiMap>
#include "shaperepresentation.h"
class vtkActor2D;
class vtkRenderWindowInteractor;
class vtkPropPicker;


namespace udg {

/**
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/

//     class ShapeRepresentation;
    class Q2DViewer;
    class PointRepresentation;

class ShapeTool : public Tool
{
public:
    ///comportaments que pot tractar la ShapeTool
    enum { POINT, POLYLINE, DISTANCE, ELLIPSE, TEXTAREA, TRACE, ARROW, NONE };
    
    ShapeTool( Q2DViewer *viewer = 0, QObject *parent = 0 , const char *name = 0 );

    ~ShapeTool();
    
    /// tracta els events que succeeixen dins de l'escena
    void handleEvent( unsigned long eventID );
    
private:
    ///Objecte per a dibuixar punts
    PointRepresentation *m_point;
    
    /**
    map de ShapeRepresentations:
    Utilitzarem un QMultiMap per guardar les diferents ShapeRepresentations que anem dibuixant. La diferència entre QMap 
    i QMultiMap és que aquest últim permet guardar més d'un objecte amb la mateixa clau, és a dir, tenir més d'una entrada 
    per a una mateixa clau. Utilitzarem la llesca on s'ha dibuixat la ShapeRepresentation com a clau del QMultiMap i la 
    ShapeRepresentation dibuixada serà la dada. Així, quan volguem totes les ShapeRepresentations de la llesca "x", crearem 
    una llista amb totes les files que tenen per clau a "x".
        
        Representació:
    
    [nº de llesca][ShapeRepresentation ]
    [     1      ][         s1         ]
    [     1      ][         s2         ]
    [     2      ][         s3         ]
    [     2      ][         s4         ]
    ...
     */
    QMultiMap<int, ShapeRepresentation*> m_representationShapesMap;    
    
    ///picker de l'escena
    vtkPropPicker               *m_picker;
    
    ///visor sobre el que es programa la tool
    Q2DViewer                   *m_viewer;
    
    ///actor que representa la ShapeRepresentation que ha rebut l'últim focus (és a dir, per l'últim que hem passat per sobre)
    vtkActor2D                  *m_focusedActor;
            
    ///actor que representa l'última ShapeRepresentation seleccionada (amb el botó dret)
    vtkActor2D                  *m_selectedActor;
    
    double m_startPosition[3];
    double m_endPosition[3];
    
    ///compatador de clicks del botó esquerre
    int m_nclicks;
    
    ///dibuixa els extrems d'una determinada ShapeRepresentation. 
    void drawEndsOfShapeRepresentation();
    
    ///oculta els extrems de la distància que ha sigut seleccionada.
    void hideEndsOfShapeRepresentation();
    
    private slots: 
    /// Comença l'anotació 
    void startAnnotation();

    /// simula l'anotació
    void doSimulation( double position[4] );

    /// Atura l'estat d'anotació de la ShapeRepresentation
    void endAnnotation();
    
    /// Selecciona la ShapeRepresentation sobre la que estem.
    void selectShapeRepresentation();
    
    /** 
        És el mètode que s'executa quan hem seleccionat el comportament d'ARROW (selecció) i ens movem 
        per sobre la imatge. Està pendent de mirar si passa per sobre d'alguna ShapeRepresentation 
        i si és així, la resalta per saber que la tenim seleccionada.
     */
    void waitingForAction();
            
    ///dibuixa totes les ShapeRepresentations de la llesca que demanem per paràmetre
    void drawShapesOfCurrentSlice( int );
    
    ///anota al visor la representació del punt passat per paràmetre i l'insereix al map de la classe
    void annotatePointRepresentation( double position[2] );
    void annotatePointRepresentation( double x, double y );
    
    
    ///ens dibuixa una el·lipse
    void annotateEllipseRepresentation( double position[2] );
};

}

#endif
