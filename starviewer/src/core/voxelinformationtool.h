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

class QViewer;
class Q2DViewer;

/**
Tool per mostrar la informació del voxel (coordenades i valor ) en un visualitzador 2D

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class VoxelInformationTool : public Tool
{
Q_OBJECT
public:
    VoxelInformationTool( QViewer *viewer, QObject *parent = 0 );

    ~VoxelInformationTool();

    void handleEvent( unsigned long eventID );

private slots:
    /// Actualitza la informació de voxel que ha de mostrar el caption
    /// d'acord amb la posició on estigui el cursor
    void updateVoxelInformation();

private:
    /// Crear l'actor on es mostrarà la informació del voxel
    void createCaptionActor();

    /// Ens retorna les dimensions de la pantalla
    int *viewportDimensions();

    ///ens dóna la posició corretgida del caption
    void correctPositionOfCaption( int position[2] );

    ///Les següents 3 funcions ens diuen si excedim els límits del viewport
    bool captionExceedsViewportTopLimit();
    bool captionExceedsViewportRightLimit();
    bool captionExceedsViewportLimits();

    ///situa el text del caption
    void placeText( double textPosition[3] );

    ///calcula la profunditat segons la vista i la llesca en la que estem.
    void depthAccordingViewAndSlice( double xyz[3] );

private:
    /// 2DViewer amb el que operem
    Q2DViewer *m_2DViewer;

    /// L'actor per mostrar les annotacions de voxel
    vtkCaptionActor2D *m_voxelInformationCaption;
};

}

#endif
