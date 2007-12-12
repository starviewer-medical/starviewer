/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGOLDDISTANCETOOL_H
#define UDGOLDDISTANCETOOL_H

#include "oldtool.h"
#include <QSet>

//Forward declarations
class vtkRenderWindowInteractor;

namespace udg {

class Q2DViewer;
class Distance;
class DistanceRepresentation;
class OldDistanceToolData;
class DrawingPrimitive;

/**
    @author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/

class OldDistanceTool : public OldTool
{
    Q_OBJECT
public:
    ///definim un tipus nom de dades
    typedef QSet< DrawingPrimitive* > PrimitivesSet;

    /// estats de la tool
    enum { NONE , ANNOTATING, MOVINGPOINT };

    /// enumeració per saber quin dels dos punts de la distància seleccionada és el més proper
    enum { NOTHINGSELECTED, FIRST, SECOND };


    OldDistanceTool( Q2DViewer *viewer , QObject *parent = 0 );

    ~OldDistanceTool();

    /// tracta els events que succeeixen dins de l'escena
    void handleEvent( unsigned long eventID );

private:
    ///ens assigna els atributs als objectes OldDistanceToolData i DistanceRepresentation de la distància seleccionada
    void createSelectedDistanceData( PrimitivesSet *primitiveSet );

    ///actualitza la posició del primer punt de la distància seleccionada
    void moveFirstPoint();

    ///actualitza la posició del segon punt de la distància seleccionada
    void moveSecondPoint();

    ///calcula quin punt de la distància seleccionada, és més proper a on es produeix l'event que el crida.
    void getNearestPointOfSelectedDistance();

    ///mètode per a respondre als events de teclat
    void answerToKeyEvent();

private slots:
    /// Comença l'anotació de la distància
    void startDistanceAnnotation();

    /// simula la nova distància
    void doDistanceSimulation();

    /// Calcula la nova distància i després atura l'estat d'anotació de distància
    void endDistanceAnnotation();

private:
    ///visor sobre el que es programa la tool
    Q2DViewer *m_2DViewer;

    /// valors per controlar l'anotació de les distàncies
    double m_distanceStartPosition[3], m_distanceCurrentPosition[3];

    /// atribut per saber quin dels dos punts de la distància seleccionada és el més proper
    int m_nearestPoint;

    ///objecte per a crear distàncies noves
    DistanceRepresentation *m_distanceRepresentation;

    ///objectes que ens servirà per crear la distància que l'usuari ha seleccionat
    DistanceRepresentation *m_selectedDistanceRepresentation;
    OldDistanceToolData *m_selectedDistanceToolData;

    ///ens permet conèixer si s'han obtingut primitives correctes com a distància seleccionada
    bool m_correctData;

    ///ens permetrà controlar si la tecla Ctrl per a seleccionar distàncies està polsada o no.
    bool m_isCtrlPressed;

    ///ens permet controlar el mode d'anotació: amb 2 clicks o clicant-arrossegant-alliberant
    QString m_annotationMode;

    ///ens permet emmagatzemar l'última acció que s'ha fet amb el botó esquerre.
    QString m_lastLeftButtonAction;
    };
}

#endif
