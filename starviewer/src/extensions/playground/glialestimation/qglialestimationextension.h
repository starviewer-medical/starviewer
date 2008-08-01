/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGQGLIALESTIMATION_H
#define UDGQGLIALESTIMATION_H

#include "ui_qglialestimationextensionbase.h"

#include <QString>

// FWD declarations
class QAction;
class QToolBar;
class vtkImageMask;
class vtkActor;
class vtkImageActor;

namespace udg {

// FWD declarations
class Volume;
class ToolsActionFactory;
class ToolManager;
class StrokeSegmentationMethod;

/**
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class QGlialEstimationExtension : public QWidget , private ::Ui::QGlialEstimationExtensionBase {
Q_OBJECT
public:
    QGlialEstimationExtension( QWidget *parent = 0 );
    ~QGlialEstimationExtension();

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

private slots:

    void computeTTP( );
    void computeCBV( );

     /// gestiona els events del m_2DView
    void glialEventHandler( unsigned long id );

    ///connecta amb el canvi de window level
    void createColorMap( double window, double level );

    ///aplica un filtre al mapa
    void applyFilterMapImage( );

    ///aplica el registre entre dues imatges
    void applyRegistration( );

    /// gestiona els events del botó esquerre
    void leftButtonEventHandler();

    /// Veure la màscara de T1
    void viewT1Thresholds(int value);
    void setT1MaskOpacity( int opacity );
    void setT1SeedPosition ( );
    void applyT1Segmentation();

    /// desactiva el booleà que ens diu si està el botó esquerra apretat
    void setLeftButtonOff( );

    /// Canvia l'opacitat de les imatges del registre
    void setRegistrationOpacity(int op);

    /// Visualitza el menú que permet escollir la sèrie del DSC
    void contextMenuT1Release();
    void contextMenuPerfuRelease();
    void contextMenuFLAIRRelease();
    void contextMenuDifuRelease();
    void contextMenuSpectrumRelease();
    void contextMenuEvent(QContextMenuEvent *event);
    void setSeries(Series *series);


private:
    enum GlialImage{ T1, perfu, FLAIR, difu, spectrum };
    int m_imageGlialtype;

    /// El volum principal
    Volume *m_mainVolume;
    Volume *m_T1Volume;
    Volume *m_T1MaskVolume;
    Volume *m_perfuVolume;
    Volume* m_mapVolume;
    Volume *m_FLAIRVolume;
    Volume *m_difuVolume;
    Volume* m_spectrumVolume;
    Volume* m_registeredVolume;

    vtkImageActor *m_registeredOverlay;

    StrokeSegmentationMethod *m_segmentationMethod;
    int m_T1Cont;
    double m_T1VolumeCont;

    bool m_isLeftButtonPressed;

    int m_minValue, m_maxValue;
    int m_minT1Value, m_maxT1Value;
    int m_insideValue, m_outsideValue;
    int m_mapMax, m_mapMin;
    double m_seedT1Position[3];

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
