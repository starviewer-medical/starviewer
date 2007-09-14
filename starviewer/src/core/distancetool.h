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
#include <QMultiMap>


//Forward declarations
class vtkRenderWindowInteractor;

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
    ///definim un tipus nom de dades
    typedef QSet< DrawingPrimitive* > PrimitivesSet;

    ///ens assigna els atributs als objectes DistanceToolData i DistanceRepresentation de la distància seleccionada
    void createSelectedDistanceData( PrimitivesSet *primitiveSet );

    ///actualitza la posició del primer punt de la distància seleccionada
    void moveFirstPoint();

    ///actualitza la posició del segon punt de la distància seleccionada
    void moveSecondPoint();

    ///calcula quin punt de la distància seleccionada, és més proper a on es produeix l'event que el crida.
    void getNearestPointOfSelectedDistance();
    
    ///mètode per a respondre als events de teclat
    void answerToKeyEvent();

    ///visor sobre el que es programa la tool
    Q2DViewer *m_2DViewer;

    /// valors per controlar l'anotació de les distàncies
    double m_distanceStartPosition[3], m_distanceCurrentPosition[3];

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
    };
}

#endif
