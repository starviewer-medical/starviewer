/***************************************************************************
 *   Copyright (C) 2007 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/



#ifndef UDGQOPTIMALVIEWPOINTEXTENSION_H
#define UDGQOPTIMALVIEWPOINTEXTENSION_H



#include "ui_qoptimalviewpointextensionbase.h"



namespace udg {



class OptimalViewpointDirector;
class OptimalViewpointParameters;
class OptimalViewpointInputParametersForm;

class Volume;



/**
 * ...
 *
 * @author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/

class QOptimalViewpointExtension : public QWidget, private ::Ui::QOptimalViewpointExtensionBase {

    Q_OBJECT

public:

    QOptimalViewpointExtension( QWidget * parent = 0 );
    ~QOptimalViewpointExtension();

    void setInput( Volume * input );

private:

    OptimalViewpointDirector * m_optimalViewpointDirector;
    OptimalViewpointParameters * m_optimalViewpointParameters;
    OptimalViewpointInputParametersForm * m_optimalViewpointInputParametersForm;



private:


    /// Registration parameters
    static const double REGISTRATION_FIXED_STANDARD_DEVIATION = 0.4;
    static const double REGISTRATION_MOVING_STANDARD_DEVIATION = 0.4;
    static const int REGISTRATION_NUMBER_OF_SPACIAL_SAMPLES = 200;
    static const int REGISTRATION_FIXED_VARIANCE = 2;
    static const int REGISTRATION_MOVING_VARIANCE = 2;
    static const int REGISTRATION_MAXIMUM_STEP = 1;
    static const double REGISTRATION_MINIMUM_STEP = 0.001;
    static const int REGISTRATION_NUMBER_OF_ITERATIONS = 300;


    /// Volum d'entrada de difusió
    Volume * m_diffusionInputVolume;
    /// Volum d'entrada de perfusió
    Volume * m_perfusionInputVolume;
    /// Volum principal de difusió
    Volume * m_diffusionMainVolume;
    /// Volum principal de perfusió
    Volume * m_perfusionMainVolume;
    /// Volum de difusió amb valors entre 0 i 255
    Volume * m_diffusionRescaledVolume;
    /// Volum de perfusió amb valors entre 0 i 255
    Volume * m_perfusionRescaledVolume;

    Volume * m_activedMaskVolume;
    Volume * m_strokeMaskVolume;
    Volume * m_ventriclesMaskVolume;

    Volume * m_blackpointEstimatedVolume;
    Volume * m_penombraMaskVolume;



    /// Stroke segmentation
    double m_strokeVolume;


    /// Penombra segmentation
    double m_penombraVolume;

    double m_seedPosition[3];

    bool m_isPaint;
    bool m_isErase;
    bool m_isEraseSlice;
    bool m_isLeftButtonPressed;

    /// Accions
    QAction * m_slicingAction;
    QAction * m_windowLevelAction;
    QAction * m_zoomAction;
    QAction * m_moveAction;
    QAction * m_rotateClockWiseAction;
    QAction * m_editorAction;
    QAction * m_voxelInformationAction;
    QAction * m_seedAction;
    QAction * m_lesionViewAction;
    QAction * m_ventriclesViewAction;

    /// Grups de botons en format exclusiu
    QActionGroup * m_toolsActionGroup;
    QActionGroup * m_viewOverlayActionGroup;


    /// Crea les accions
    /// \TODO 'pujar' al pare com a mètode virtual com a Extensions? [hauria de ser protected]
    void createActions();

    /// Crea la ToolBar d'eines i altres si n'hi ha
    /// \TODO 'pujar' al pare com a mètode virtual com a Extensions? [hauria de ser protected]
    void createToolBars();

    /// Crea les connexions entre signals i slots
    void createConnections();

    /// Carrega la configuració
    void readSettings();

    /// Desa la configuració
    void writeSettings();

    /// Calcula el volum de la màscara d'stroke
    double calculateStrokeVolume();




signals:

    void openPerfusionImage();

};

} // end namespace udg



#endif
