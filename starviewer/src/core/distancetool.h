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

class vtkRenderWindowInteractor;
class vtkPropPicker;
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
    Distance* getDistanceFromActor2D( vtkActor2D* );

private:
    
    void choosePicker();  
            
    void createAction(){};
    
    void giveMeTheNearestPoint();
    
    void moveSelectedPoint();
    
    void updateSelectedPoint();
    
    vtkCaptionActor2D* createACaption2D();
            
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
    QMultiMap<int, vtkActor2D*> m_distancesOfAxialViewMap;   
    QMultiMap<int, vtkActor2D*> m_distancesOfCoronalViewMap;  
    QMultiMap<int, vtkActor2D*> m_distancesOfSagittalViewMap;                   

    ///interactor de l'escena
    vtkRenderWindowInteractor   *m_interactor;

    ///pickers de l'escena
    vtkPropPicker               *m_axialPicker;
    vtkPropPicker               *m_coronalPicker;
    vtkPropPicker               *m_sagittalPicker;
    vtkPropPicker               *m_currentPicker;
    

    ///visor sobre el que es programa la tool
    Q2DViewer                   *m_2DViewer;

    ///objecte per a dibuixar la línia que ens determinarà la distància que estem anotant
    vtkLineSource               *m_distanceLine;
    
    ///actor que representa l'última distància seleccionada (amb el botó dret)
    vtkActor2D                  *m_selectedDistance;
    
    ///guarda una referència a la distància més propera
    vtkProp *m_nearestProp;
    
    ///assemblatge que conté la distància que hem seleccionat amb la seva corresponent etiqueta d'anotació
    vtkPropAssembly *m_selectedAssembly;

    ///actor per a representar la línia de la distància
    vtkActor2D                   *m_lineActor;

    ///mapejador de la línia de distància
    vtkPolyDataMapper2D          *m_lineMapper;

    ///actor per a mostrar la distància anotada
    vtkCaptionActor2D            *m_textDistanceActor;

    /// valors per controlar l'anotació de les distàncies 
    double                        m_distanceStartPosition[3], m_distanceCurrentPosition[3];

    /// controlador dels punts de la distància
    bool                         m_firstPointLocated;
    
    /// enter per saber quin dels dos punts d'una distància hem seleccionat
    int                         m_pointOfDistance;

    ///indica si estem a sobre d'alguna distància de les anotades
    bool m_somePropFocused;

    ///indica si hem seleccionat alguna de les distànces anotades
    bool m_somePropSelected;

private slots: 
    /// Comença l'anotació de la distància
    void startDistanceAnnotation();

    /// simula la nova distància
    void doDistanceSimulation();

    /// Calcula la nova distància i després atura l'estat d'anotació de distància
    void endDistanceAnnotation();

    /// Selecciona la distància sobre la que estem, com a efecte de la polsació del botó dret
    void selectDistance();
    
    /// ens troba la distància més propera (amb un cert marge) a un punt determinat
    bool giveMeNearestDistance();

    /** 
        És el mètode que s'executa quan no s'està anotant cap distància i ens movem per sobre la imatge.
        Està pendent de mirar si passa per sobre d'alguna distància i si és així, la resalta per saber
        quina distància tenim seleccionada si escollíssim borrar amb el botó dret.
    */
    void waitingForAction();
    
    ///dibuixa totes les distàncies de la llesca que demanem per paràmetre
    void drawDistancesOfSlice( int );
    
    ///respon als events de teclat
    void answerToKeyEvent();
    
    ///mira si la llista de distàncies de la llesca actual conté la distància passada per paràmetre
    bool distancesListContainsThisDistance( vtkProp *distance, int slice );
    
    ///assigna visibilitat al prop passat per paràmetre
    void setVisibilityToThisDistanceProp( vtkProp *prop, bool visibility );
    };
}

#endif
