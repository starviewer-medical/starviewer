/**
 * TODO
 *
 * Clic per posar la llavor no funciona sempre a la primera
 *
 * Les llesques canvien de 2 en 2 (a vegades)
 *
 * Queda un quadrat verd de l'ajustador de màscara
 *
 * Que es mantingui obert el mateix DICOMDIR?????????????????
 *  - Fet (a mitges)
 *
 * Es pot posar llavor a perfu
 */



/***************************************************************************
 *   Copyright (C) 2007 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/



#ifndef UDGQDIFFUSIONPERFUSIONSEGMENTATIONEXTENSION_H
#define UDGQDIFFUSIONPERFUSIONSEGMENTATIONEXTENSION_H



#include "ui_qdifuperfuextensionbase.h"

#include "itkRegistre3DAffine.h"
#include "volume.h"



// Forward declarations

class QToolBar;

class vtkActor;
class vtkImageActor;



namespace udg {



// Forward declarations
class StrokeSegmentationMethod;
class ToolsActionFactory;



/**
 * Extensió que permet segmentar una lesió i calcular-ne el volum a partir de les imatges de difusió
 * i perfusió.
 *
 * @author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/

class QDifuPerfuSegmentationExtension : public QWidget, private ::Ui::QDifuPerfuExtensionBase {

    Q_OBJECT


public:

    QDifuPerfuSegmentationExtension( QWidget * parent = 0 );
    ~QDifuPerfuSegmentationExtension();


public slots:

    /// Li assigna el volum principal
    void setDiffusionInput( Volume * input );
    void setPerfusionInput( Volume * input );

    /// Executa l'algorisme de segmentació
    void applyStrokeSegmentation();
    void applyVentriclesMethod();
    void applyRegistration();
    void computeBlackpointEstimation();
    void applyPenombraSegmentation();


private:

    typedef Volume::ItkImageType ItkImageType;
    typedef itkRegistre3DAffine< ItkImageType, ItkImageType >::TransformType TransformType;


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

    ItkImageType::PixelType m_diffusionMinValue, m_diffusionMaxValue;
    ItkImageType::PixelType m_perfusionMinValue, m_perfusionMaxValue;

    vtkImageActor * m_perfusionOverlay;

    /// Stroke segmentation
    StrokeSegmentationMethod * m_strokeSegmentationMethod;
    double m_strokeVolume;

    /// Registration
    TransformType::Pointer m_registerTransform;

    /// Penombra segmentation
    double m_penombraVolume;

    double m_seedPosition[3];

    bool m_isPaint;
    bool m_isErase;
    bool m_isEraseSlice;
    bool m_isLeftButtonPressed;

    vtkActor * m_squareActor;

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

    ToolsActionFactory * m_actionFactory;

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


private slots:

    void setMaxDiffusionImage( int max );
    void setDiffusionImage( int index );
    void setMaxPerfusionImage( int max );
    void setPerfusionImage( int index );

    /// actualitza el valor llindar baix
    void setStrokeLowerValue( int x );
    /// actualitza el valor llindar alt
    void setStrokeUpperValue( int x );

    /// Visualitza la màscara donats uns thresholds
    void viewThresholds();

    /// visualitza la informació de la llavor del mètode de segmentació
    void setSeedPosition();

    /// Refresca el resultat del volum
    void updateStrokeVolume();

    void applyFilterDiffusionImage();
    







    
     /// gestiona els events del m_2DView
    void strokeEventHandler( unsigned long id );

    /// gestiona els events del botó esquerre
    void leftButtonEventHandler( );



     /// determina la llavor del m�ode de segmentaci�    
    void setEditorPoint( );

    /// desactiva el boole�que ens diu si est�el bot�esquerra apretat
    void setLeftButtonOff( );
    


    /// Canvia la opacitat de la màscara (difusió)
    void setDiffusionOpacity( int opacity );

    /// Canvia la opacitat de la màscara (perfusió)
    void setPerfusionOpacity( int opacity );

    /// Canvia a la opci�esborrar 
    void setErase();

    /// Canvia a la opci�pintar
    void setPaint();

    /// Canvia a la opci�esborrar llesca
    void setEraseSlice();

    /// Dibuixa el cursor en la forma del pinzell
    void setPaintCursor( );

    void eraseMask(int size);
    void paintMask(int size);
    void eraseSliceMask();



    /// Visualitza els diferents overlays
    void viewLesionOverlay();
//     void viewEdemaOverlay();
    void viewVentriclesOverlay();

    /// Desa la màscara que s'està visualitzant
//     void saveActivedMaskVolume();

/*    /// sincronitza les llesques de les s?ies que es visualitzen
    void synchronizeSlices( bool ok );

    /// ens permet escollir una nova s?ie per a comparar
    void chooseNewSerie();
    
  signals:
    /// Aquest senyal s'emetr?quan es vulgui canviar de s?ie per comparar
    void newSerie();
*/


    // [temporal] el farem servir mentre no s'actualitzi la tècnica d'overlay del Q2DViewer
    void setPerfusionSlice( int slice );


    void synchronizeSlices( bool sync );



signals:

    void openPerfusionImage();

};

} // end namespace udg



#endif
