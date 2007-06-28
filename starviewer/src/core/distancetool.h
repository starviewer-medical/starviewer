/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGDISTANCETOOL_H
#define UDGDISTANCETOOL_H

#include "tool.h"

#include <vtkLineSource.h>
#include <vtkPropAssembly.h>
#include <vtkPropCollection.h>
#include <vtkCaptionActor2D.h>

#include <QMultiMap>
#include <QColor>

class vtkRenderWindowInteractor;
class vtkLineSource;
class vtkActor2D;
class vtkPolyDataMapper2D;
class vtkDiskSource;

namespace udg {

class Q2DViewer;
class Distance;
class AssemblyAndLineObject;

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

    void updateSelectedPoint();

    vtkCaptionActor2D* createACaption2D();

    ///actualitza la posició del primer punt de la distància seleccionada
    void updateFirstPoint();

    ///actualitza la posició del segon punt de la distància seleccionada
    void updateSecondPoint();

    ///calcula quin punt de la distància seleccionada, és més proper a on es produeix l'event que el crida.
    void getNearestPointOfSelectedDistance();

    /// Donat un punt 3D en coordenades de món, es retorna l'actor més proper, dins del llindar m_tolerance, de la llesca actual
    AssemblyAndLineObject *getNearestAssembly( double point[3] );

    /// Ressalta l'actor 2D que estigui més a prop de la posició actual del cursor
    void highlightNearestAssembly();

    /// Pinta un assembly del color que li diem
    void setDistanceColor( AssemblyAndLineObject* assembly, QColor color );

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
    QMultiMap<int, AssemblyAndLineObject*> m_distancesOfAxialViewMap;
    QMultiMap<int, AssemblyAndLineObject*> m_distancesOfCoronalViewMap;
    QMultiMap<int, AssemblyAndLineObject*> m_distancesOfSagittalViewMap;

    ///visor sobre el que es programa la tool
    Q2DViewer *m_2DViewer;

    ///objecte per a dibuixar la línia que ens determinarà la distància que estem anotant
    vtkLineSource *m_distanceLine;

    ///assemblatge que conté la distància que hem seleccionat amb la seva corresponent etiqueta d'anotació
    AssemblyAndLineObject *m_selectedAssembly;

    ///actor per a representar la línia de la distància
    vtkActor2D *m_lineActor;

    ///mapejador de la línia de distància
    vtkPolyDataMapper2D *m_lineMapper;

    ///actor per a mostrar la distància anotada
    vtkCaptionActor2D *m_textDistanceActor;

    /// valors per controlar l'anotació de les distàncies
    double m_distanceStartPosition[3], m_distanceCurrentPosition[3];

    /// Últim actor que s'ha ressaltat
    AssemblyAndLineObject *m_previousHighlightedAssembly;

    /// Llesca actual on estem pintant les distàncies. Ens serveix per controlar els actors que s'han de dibuixar o no al canviar de llesca
    int m_currentSlice;

    /// atribut per saber quin dels dos punts de la distància seleccionada és el més proper
    int m_nearestPoint;

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

    /// deselecciona la distància que està seleccionada en el moment de cridar l'slot
    void unselectDistance();
};

//CLASSE INTERNA DE DISTANCETOOL

class AssemblyAndLineObject
{
    ///Ens permet fer agrupacions d'un assembly i el vtkLineSource associat per tenir accés més directe a la distància.
    public:

        AssemblyAndLineObject( vtkPropAssembly *assembly , vtkLineSource *line )
        {m_assembly = assembly; m_line = line;};

        ~AssemblyAndLineObject()
        {m_assembly->Delete(); m_line->Delete();};

        //MÈTODES

        ///ens retorna l'assembly
        vtkPropAssembly* getAssembly()
        {return m_assembly;};

        ///ens retorna el vtkLineSource
        vtkLineSource* getLine()
        {return m_line;};

        ///ens retorna el vtkCaptionActor2D que hi ha dins de l'assembly
        vtkCaptionActor2D* getCaption()
        {
            vtkPropCollection *assemblyCollection = m_assembly->GetParts();
            return (vtkCaptionActor2D::SafeDownCast ( assemblyCollection->GetItemAsObject( 1 ) ) );
        };

        ///ens permet associar l'assembly
        void setAssembly( vtkPropAssembly *assembly )
        {m_assembly = assembly;};

        ///ens permet associar el vtkLineSource
        void setLine( vtkLineSource *line )
        {m_line = line;};

    private:

        vtkPropAssembly *m_assembly;

        vtkLineSource *m_line;
};
}

#endif
