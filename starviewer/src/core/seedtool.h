/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gr�fics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGSEEDTOOL_H
#define UDGSEEDTOOL_H

#include "tool.h"

class vtkActor;
class vtkSphereSource;
class vtkPolyDataMapper;

namespace udg {

class Q2DViewer;

/**
Tool que serveix per posar llavors en un visor 2D

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class SeedTool : public Tool
{
Q_OBJECT
public:
    /// estats de la tool
    enum { NONE , SEEDING };

    SeedTool(Q2DViewer *viewer, QObject *parent = 0 );

    ~SeedTool();

    void handleEvent( unsigned long eventID );

    double* getSeedPosition(){return m_seedPosition;}

    int getSeedSlice(){return m_seedSlice;}

private:
    //M�tode deprecated
    void createAction();

    Q2DViewer *m_2DViewer;

    int m_seedSlice;

    ///Actor que dibuixa la llavor
    vtkActor *m_pointActor;
    
    ///esfera que representa en punt de la llavor
    vtkSphereSource *m_point;
    
    ///mapejador del punt
    vtkPolyDataMapper *m_pointMapper;

    /// Coordenades on s'ha posat la llavor
    double m_seedPosition[3];


/// \TODO potser aquests m�todes slots passen a ser p�blics
private slots:
    /// Posem la llavor
    void setSeed( );

    /// Calcula la llesca que s'ha de moure
    void doSeeding();

    /// Atura l'estat d'slicing
    void endSeeding();

    /// Pinta/despinta la llavor
    void sliceChanged( int s );

};

}

#endif
