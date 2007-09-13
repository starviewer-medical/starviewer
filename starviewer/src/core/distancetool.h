/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGDISTANCETOOL_H
#define UDGDISTANCETOOL_H

#include "tool.h"

#include <QSet>
// #include <vtkPropAssembly.h>
// #include <vtkPropCollection.h>
// #include <vtkCaptionActor2D.h>

#include <QMultiMap>
// #include <QColor>

class vtkRenderWindowInteractor;
// class vtkLineSource;
// class vtkActor2D;
// class vtkPolyDataMapper2D;

namespace udg {

class Q2DViewer;
class Distance;
class DistanceRepresentation;
class DistanceToolData;
class DrawingPrimitive;

/**
    @author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/

class DistanceTool : public Tool
{
    Q_OBJECT
public:

    /// estats de la tool
    enum { NONE , ANNOTATING, MOVINGPOINT };

    /// enumeració per saber quin dels dos punts de la distància seleccionada és el més proper
    enum { NOTHINGSELECTED, FIRST, SECOND };


    DistanceTool( Q2DViewer *viewer , QObject *parent = 0 );

    ~DistanceTool();

    /// tracta els events que succeeixen dins de l'escena
    void handleEvent( unsigned long eventID );

private:
    //definim un tipus nom de dades
    typedef QSet< DrawingPrimitive* > PrimitivesSet;

    ///ens assigna els atributs als objectes DistanceToolData i DistanceRepresentation de la distància seleccionada
    void createSelectedDistanceData( PrimitivesSet *primitiveSet );
//     void updateSelectedPoint();

//     vtkCaptionActor2D* createACaption2D();

    ///actualitza la posició del primer punt de la distància seleccionada
    void moveFirstPoint();

    ///actualitza la posició del segon punt de la distància seleccionada
    void moveSecondPoint();

    ///calcula quin punt de la distància seleccionada, és més proper a on es produeix l'event que el crida.
    void getNearestPointOfSelectedDistance();

    /// Donat un punt 3D en coordenades de món, es retorna l'actor més proper, dins del llindar m_tolerance, de la llesca actual
//     AssemblyAndLineObject *getNearestAssembly( double point[3] );

    /// Ressalta l'actor 2D que estigui més a prop de la posició actual del cursor
//     void highlightNearestAssembly();

    /// Pinta un assembly del color que li diem
//     void setDistanceColor( AssemblyAndLineObject* assembly, QColor color );

    ///visor sobre el que es programa la tool
    Q2DViewer *m_2DViewer;

    ///objecte per a dibuixar la línia que ens determinarà la distància que estem anotant
//     vtkLineSource *m_distanceLine;

    ///assemblatge que conté la distància que hem seleccionat amb la seva corresponent etiqueta d'anotació
//     AssemblyAndLineObject *m_selectedAssembly;

    ///actor per a representar la línia de la distància
//     vtkActor2D *m_lineActor;

    ///mapejador de la línia de distància
//     vtkPolyDataMapper2D *m_lineMapper;

    ///actor per a mostrar la distància anotada
//     vtkCaptionActor2D *m_textDistanceActor;

    /// valors per controlar l'anotació de les distàncies
    double m_distanceStartPosition[3], m_distanceCurrentPosition[3];

    /// Últim actor que s'ha ressaltat
//     AssemblyAndLineObject *m_previousHighlightedAssembly;

    /// Llesca actual on estem pintant les distàncies. Ens serveix per controlar els actors que s'han de dibuixar o no al canviar de llesca
    int m_currentSlice;

    /// atribut per saber quin dels dos punts de la distància seleccionada és el més proper
    int m_nearestPoint;

    /// Manté la última vista del 2DViewer. Ens servirà per controlar les distàncies visibles quan canviem de vista
    int m_lastView;
    
    ///objecte per a crear distàncies noves
    DistanceRepresentation *m_distanceRepresentation;

    ///objectes que ens servirà per crear la distància que l'usuari ha seleccionat
    DistanceRepresentation *m_selectedDistanceRepresentation;
    DistanceToolData *m_selectedDistanceToolData;
    
    ///ens permet conèixer si s'han obtingut primitives correctes com a distància seleccionada
    bool m_correctData;
    
private slots:
    /// Comença l'anotació de la distància
    void startDistanceAnnotation();

    /// simula la nova distància
    void doDistanceSimulation();

    /// Calcula la nova distància i després atura l'estat d'anotació de distància
    void endDistanceAnnotation();

    /// Selecciona la distància sobre la que estem, com a efecte de la polsació del botó dret
//     void selectDistance();

    /// Dibuixa les distàncies de la llesca que demanem per paràmetre i fa invisibles les altres
//     void drawDistancesOfSlice( int );

    ///respon als events de teclat
    void answerToKeyEvent();

    /// deselecciona la distància que està seleccionada en el moment de cridar l'slot
//     void unselectDistance();
};
}

#endif
