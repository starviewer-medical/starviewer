/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGDISTANCETOOL_H
#define UDGDISTANCETOOL_H

#include "tool.h"
#include "distance.h"
#include <list>

// Forward declarations
class vtkActorCollection;
class vtkActor;
class vtkPolyDataMapper;
class vtkRenderer;
class vtkLineSource;

namespace udg {

// Forward declarations
class QViewer;

/**
Tool per a mesurar distàncies

@author Grup de Gràfics de Girona  ( GGG )
*/
class DistanceTool : public Tool{
Q_OBJECT
public:
    DistanceTool( QViewer *viewer, QObject *parent = 0, const char *name = 0);
    ~DistanceTool();

    enum State{ Begin, FirstPointCandidate , FirstPointChosen , SecondPointCandidate };
    
    virtual void dispatchEvent( EventIdType event );
    
    /// retorna la capa on es pinten tots els elements visibles de la tool
    vtkRenderer *getRendererLayer(){ return m_distanceRenderer; };
    
    void setRendererLayer( vtkRenderer *renderer ){ m_distanceRenderer = renderer; }
private:
    /// Estat de la Tool
    State m_state;
    /// Mesura que s'està realitzant en aquell moment
    Distance m_distance; 
    /// Llista de distàncies recolectades
    std::list< Distance > m_distanceList;
    /// Visor amb que estem treballant
    QViewer *m_viewer;
    /// Dibuixa sobre l'escena els marcadors que indiquen la distància [1r punt, 2n punt, recta entre aquests, label amb la distància ]
    // \TODO es podria fer una classe ToolLayer :: DistanceToolLayer que agrupi tot el referent a 'pintar'
    void drawDistance();
    /// Conté tots els actors a pintar en pantalla que representin les distàncies
    vtkActorCollection *m_distanceActorsCollection;
    /// Actors diversos que representen la distància que estem mesurant
    vtkActor *m_firstPointActor, *m_secondPointActor, *m_distanceLineActor;
    vtkLineSource *m_lineSource;
    /// Els mappers corresponents
    vtkPolyDataMapper *m_pointMapper, *m_lineMapper;
    
    
    /// El renderer per aquests elements
    vtkRenderer *m_distanceRenderer;
    
};

};  //  end  namespace udg 

#endif
