/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGDISTANCETOOL_H
#define UDGDISTANCETOOL_H

#include "tool.h"

#include <QMultiMap>
#include <QColor>

class vtkRenderWindowInteractor;
class vtkLineSource;
class vtkActor;
class vtkPropAssembly;
class vtkActor2D;
class vtkPolyDataMapper;
class vtkPolyDataMapper2D;
class vtkDiskSource;
class vtkCaptionActor2D;
class vtkProp;

namespace udg {

class Q2DViewer;
class Distance;

/**
    @author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/

class DistanceTool : public Tool
{
    Q_OBJECT
public:

    /// estats de la tool
    enum { NONE , ANNOTATING, MOVINGPOINT };

    DistanceTool( Q2DViewer *viewer , QObject *parent = 0 , const char *name = 0 );

    ~DistanceTool();

    /// tracta els events que succeeixen dins de l'escena
    void handleEvent( unsigned long eventID );

    ///ens retorna la distància que conté un determinat actor2D (si és que la conté)
    Distance getDistanceFromActor2D( vtkActor2D* );

private:

    void createAction(){};

    void moveSelectedPoint();

    void updateSelectedPoint();

    vtkCaptionActor2D* createACaption2D();

    /// Donat un punt 2D en coordenades de món, es retorna l'actor més proper, dins del llindar m_tolerance, de la llesca actual
    vtkPropAssembly *getNearestAssembly( double point[2] );

    /// Ressalta l'actor 2D que estigui més a prop de la posició actual del cursor
    void highlightNearestAssembly();

    /// Pinta un assembly del color que li diem
    void setDistanceColor( vtkPropAssembly * distance , QColor color );

    /**
    map de distàncies:
        Utilitzarem un QMultiMap per guardar les distàncies. La diferència entre QMap i QMultiMap és que aquest últim permet
        guardar més d'un objecte amb la mateixa clau, és a dir, tenir més d'una entrada per a una mateixa clau. Utilitzarem
        la llesca on s'ha anotat la distància com a clau del QMultiMap i la distància anotada serà la dada. Així, quan volguem
        totes les distàncies de la llesca "x", crearem una llista amb totes les files que tenen per clau a "x".

        Representació:

        [nº de llesca][Distància]
        [     1      ][   d1    ]
        [     1      ][   d2    ]
        [     2      ][   d3    ]
        [     2      ][   d4    ]
        ...
    */
    QMultiMap<int, vtkPropAssembly*> m_distancesOfAxialViewMap;
    QMultiMap<int, vtkPropAssembly*> m_distancesOfCoronalViewMap;
    QMultiMap<int, vtkPropAssembly*> m_distancesOfSagittalViewMap;

    ///visor sobre el que es programa la tool
    Q2DViewer *m_2DViewer;

    ///objecte per a dibuixar la línia que ens determinarà la distància que estem anotant
    vtkLineSource *m_distanceLine;

    ///actor que representa l'última distància seleccionada (amb el botó dret)
    vtkActor2D *m_selectedDistance;

    ///guarda una referència a la distància més propera
    vtkProp *m_nearestProp;

    ///assemblatge que conté la distància que hem seleccionat amb la seva corresponent etiqueta d'anotació
    vtkPropAssembly *m_selectedAssembly;

    ///actor per a representar la línia de la distància
    vtkActor2D *m_lineActor;

    ///mapejador de la línia de distància
    vtkPolyDataMapper2D *m_lineMapper;

    ///actor per a mostrar la distància anotada
    vtkCaptionActor2D *m_textDistanceActor;

    /// valors per controlar l'anotació de les distàncies
    double m_distanceStartPosition[3], m_distanceCurrentPosition[3];

    /// controlador dels punts de la distància
    bool m_firstPointLocated;

    /// enter per saber quin dels dos punts d'una distància hem seleccionat
    int m_pointOfDistance;

    /// indica si estem a sobre d'alguna distància de les anotades
    bool m_somePropFocused;

    /// indica si hem seleccionat alguna de les distànces anotades
    bool m_somePropSelected;

    /// Últim actor que s'ha ressaltat
    vtkPropAssembly *m_previousHighlightedAssembly;

    /// Llesca actual on estem pintant les distàncies. Ens serveix per controlar els actors que s'han de dibuixar o no al canviar de llesca
    int m_currentSlice;

    /// Manté la última vista del 2DViewer. Ens servirà per controlar les distàncies visibles quan canviem de vista
    int m_lastView;

    /// Colors dels diferents estat de les distàncies
    const QColor HighlightColor;
    const QColor NormalColor;
    const QColor SelectedColor;

private slots:
    /// Comença l'anotació de la distància
    void startDistanceAnnotation();

    /// simula la nova distància
    void doDistanceSimulation();

    /// Calcula la nova distància i després atura l'estat d'anotació de distància
    void endDistanceAnnotation();

    /// Selecciona la distància sobre la que estem, com a efecte de la polsació del botó dret
    void selectDistance();

    /// Dibuixa les distàncies de la llesca que demanem per paràmetre i fa invisibles les altres
    void drawDistancesOfSlice( int );

    ///respon als events de teclat
    void answerToKeyEvent();
};

}

#endif
