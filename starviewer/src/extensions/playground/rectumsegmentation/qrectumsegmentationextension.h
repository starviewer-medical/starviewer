/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGQRECTUMSEGMENTATION_H
#define UDGQRECTUMSEGMENTATION_H

#include "ui_qrectumsegmentationextensionbase.h"

#include <QString>

// FWD declarations
class QAction;
class QToolBar;
class vtkImageMask;
class vtkImageThreshold;
class vtkActor;

namespace udg {

// FWD declarations
class Volume;
class rectumSegmentationMethod;
class ToolsActionFactory;
class ToolManager;

/**
    @author Laboratori de Gràfics i Imatge  ( GILab ) <vismed@ima.udg.es>
*/
class QRectumSegmentationExtension : public QWidget , private ::Ui::QRectumSegmentationExtensionBase {
Q_OBJECT
public:
    QRectumSegmentationExtension( QWidget *parent = 0 );
    ~QRectumSegmentationExtension();

public slots:
    /// Li assigna el volum principal
    void setInput( Volume *input );

    /// Executa l'algorisme de segmetnació
    void ApplyMethod();
    void ApplyFilterMainImage( );

private:
    /// crea les accions \TODO 'pujar' al pare com a m?ode virtual com a Extensions? [hauria de ser protected]
    void createActions();

    /// Crea les connexions entre signals i slots
    void createConnections();

    /// Guarda i llegeix les caracter�tiques
    void readSettings();
    void writeSettings();

private slots:
     /// gestiona els events del m_2DView
    void strokeEventHandler( unsigned long id );

    /// gestiona els events del botó esquerre
    void leftButtonEventHandler( );
    void onMouseMoveEventHandler( );
    void leftButtonReleaseHandler( );

    /// visualitza la informació de la llavor del mètode de segmentació
    void setSeedPosition( );

    void setRegionOfInterest( );
    void setMovingRegionOfInterest( );
    void setReleaseRegionOfInterest( );
    void viewRegionState(int st);

    /// desactiva el booleà que ens diu si està el botó esquerra apretat
    void setLeftButtonOff( );

    /// actualitza el valor llindar baix
    void setLowerValue( int x );

     /// actualitza el valor llindar alt
    void setUpperValue( int x );

    /// Canvia la opacitat de la màscara
    void setOpacity(int op);

    /// Calcula el volum de la màscara
    double calculateMaskVolume();

    /// Refresca el resultat del volum
    void updateVolume();

    /// Visualitza la màscara donats uns thresholds
    void viewThresholds();

    /// Visualitza els diferents overlays
    void viewLesionOverlay();

    /// Desa la màscara que s'està visualitzant
    void saveActivedMaskVolume();

    /// Desa la màscara que s'està visualitzant en format "xulo" per veure-ho en 3D
    void saveSegmentation3DVolume();

    /// Desactiva les tools en cas que s'activi una tool "externa" (ara només la ROI)
    void toolChanged( int but );

private:
    /// El volum principal
    Volume *m_mainVolume;

    /// El volum on hi guardem el resultat de la segmentació
    Volume *m_lesionMaskVolume;
    vtkImageThreshold *m_imageThreshold;

    /// El volum on hi guardem la imatge principal filtrada
    Volume *m_filteredVolume;

    /// Mètode de la segmentació
    rectumSegmentationMethod *m_segMethod;

    /// Membres de classe
    bool m_isSeed;
    QVector<double> m_seedPosition;
    bool m_isMask;

    bool m_isLeftButtonPressed;
    double m_volume;

    vtkActor *squareRegionActor;
    double m_initialRegionPoint[2];
    double m_finalRegionPoint[2];
    bool m_isRegionSet;
    bool m_isRegionSetting;

    int m_minValue, m_maxValue;
    int m_insideValue, m_outsideValue;
    int m_lowerVentriclesValue, m_upperVentriclesValue;

    /// Accions
    QAction *m_regionAction;
    QAction *m_rotateClockWiseAction;
    QAction *m_lesionViewAction;
    QAction *m_rectumViewAction;
    QAction *m_ventriclesViewAction;
    QButtonGroup *m_toolsButtonGroup;

    ///Directori on guardem les màscares
    QString m_savingMaskDirectory;

    /// Grup de botons en format exclusiu
    QActionGroup *m_viewOverlayActionGroup;

    /// Tool Manager
    ToolManager *m_toolManager;
};

} // end namespace udg

#endif
