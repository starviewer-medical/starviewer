/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGQVSIRECONSTRUCTION_H
#define UDGQVSIRECONSTRUCTION_H

#include "ui_qvsireconstructionextensionbase.h"

#include <QString>

// FWD declarations
class QAction;
class QToolBar;
class vtkImageMask;
class vtkActor;

namespace udg {

// FWD declarations
class Volume;
class ToolsActionFactory;
class ToolManager;

/**
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class QVSIReconstructionExtension : public QWidget , private Ui::QVSIReconstructionExtensionBase {
Q_OBJECT
public:
    QVSIReconstructionExtension( QWidget *parent = 0 );
    ~QVSIReconstructionExtension();

public slots:
    /// Li assigna el volum principal
    void setInput( Volume *input );

private:
    /// crea les accions \TODO 'pujar' al pare com a m?ode virtual com a Extensions? [hauria de ser protected]
    void createActions();

    /// Crea les connexions entre signals i slots
    void createConnections();

    /// Guarda i llegeix les característiques
    void readSettings();
    void writeSettings();
    bool findProbableSeries( );

    void createColorMap( );
    void createColorMap2( );

private slots:

    void computeTTP( );
    void computeCBV( );
    void computeVSI( );

     /// gestiona els events del m_2DView
    void vsiEventHandler( unsigned long id );

    ///connecta amb el canvi de window level
    void createColorMap( double window, double level );

    ///aplica un filtre al mapa
    void applyFilterMapImage( );
    void applyFilterMapImage2( );

    /// gestiona els events del botó esquerre
    void leftButtonEventHandler();

    /// desactiva el booleà que ens diu si està el botó esquerra apretat
    void setLeftButtonOff( );

    /// Visualitza el menú que permet escollir la sèrie del DSC
    void contextMenuDSCRelease();
    void contextMenuSEPreRelease();
    void contextMenuSEPostRelease();
    void contextMenuEvent(QContextMenuEvent *event);
    void setSeries(Series *series);


private:
    enum VSIImage{ DSC , SEPre, SEPost };
    int m_imageVSItype;

    /// El volum principal
    Volume *m_mainVolume;
    Volume *m_DSCVolume;
    Volume *m_SEPreVolume;
    Volume *m_SEPostVolume;
    Volume* m_mapVolume;
    Volume* m_mapVolume2;

    bool m_isLeftButtonPressed;

    int m_minValue, m_maxValue;
    int m_insideValue, m_outsideValue;
    int m_mapMax, m_mapMin;

    /// Accions
    QAction *m_slicingAction;
    QAction *m_windowLevelAction;
    QAction *m_zoomAction;
    QAction *m_moveAction;
    QAction *m_rotateClockWiseAction;
    QAction *m_voxelInformationAction;
    ToolsActionFactory *m_actionFactory;

    /// Grup de botons en format exclusiu
    QActionGroup *m_toolsActionGroup;

    /// Tool manager
    ToolManager *m_toolManager;
};

} // end namespace udg

#endif
