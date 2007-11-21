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

public slots:
    /**
     * Activa/Desactiva aquesta tool
     */
    void enable( bool enable = true );

    ///mira si cal actualitzar la informació de vòxel. Si la tool d'informació de vòxel està activada i es canvia la llesca, s'executarà aquest SLOT.
    void isNeededUpdateVoxelInformation();

    ///ens retorna les dimensions de la pantalla
    int* viewportDimensions();

private:
    void createCaptionActor();
    void updateVoxelInformation();
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
    Q2DViewer *m_2DViewer;
    bool m_isEnabled;
    vtkCaptionActor2D *m_voxelInformationCaption;
};

}

#endif
