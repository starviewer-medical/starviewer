/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGVOXELINFORMATIONTOOL_H
#define UDGVOXELINFORMATIONTOOL_H

#include "tool.h"

class vtkCaptionActor2D;

namespace udg {

class Q2DViewer;

/**
Tool per mostrar la informació del voxel (coordenades i valor ) en un visualitzador 2D

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class VoxelInformationTool : public Tool
{
Q_OBJECT
public:
    VoxelInformationTool( Q2DViewer *viewer, QObject *parent = 0 );

    ~VoxelInformationTool();

    void handleEvent( unsigned long eventID );

public slots:
    /**
     * Activa/Desactiva aquesta tool
     */
    void enable( bool enable = true );

private:
    void createCaptionActor();
    void updateVoxelInformation();

private:
    Q2DViewer *m_2DViewer;
    bool m_isEnabled;
    vtkCaptionActor2D *m_voxelInformationCaption;
};

}

#endif
