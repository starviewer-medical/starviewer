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

/**
Extensi?que s'executar?per defecte a l'obrir un model

    @author Grup de Gr?ics de Girona  ( GGG ) <vismed@ima.udg.es>
*/

class QRectumSegmentationExtension : public QWidget , private ::Ui::QRectumSegmentationExtensionBase {
Q_OBJECT
public:
    QRectumSegmentationExtension( QWidget *parent = 0 );

    ~QRectumSegmentationExtension();

    /// Obtenim la ToolBar d'eines de l'extensi?\TODO 'pujar' al pare com a m?ode com a Extensions?
    QToolBar *getToolsToolBar() const { return m_toolsToolBar; };

    /// Omple la ToolBar amb les eines de l'extensi?\TODO 'pujar' al pare com a m?ode com a Extensions?
    //void populateToolBar( QToolBar *toolbar );

public slots:

    /// Li assigna el volum principal
    void setInput( Volume *input );

    /// Executa l'algorisme de segmetnaci�
    void ApplyMethod();
    void ApplyFilterMainImage( );

/*  void changeViewToSagital();
    void changeViewToCoronal();

    /// Canvia el layout de visor singular/doble
    void changeViewToSingle();
    void changeViewToDouble();

    /// Li assigna el volum secundari. Aquest m�ode nom? ? de conveni?cia i ? temporal
    void setSecondInput( Volume *input );
    */
private:
    /// tipus d'edició dels models
    enum EditorType{ NoEditor , Paint , Erase , EraseSlice , EraseRegion };

    /// crea les accions \TODO 'pujar' al pare com a m?ode virtual com a Extensions? [hauria de ser protected]
    void createActions();

    /// Crea la ToolBar d'eines i altres si n'hi ha \TODO 'pujar' al pare com a m?ode virtual com a Extensions? [hauria de ser protected]
    void createToolBars();

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


    /// visualitza la informaci�de la llavor del m�ode de segmentaci�
    void setSeedPosition( );

    void setRegionOfInterest( );
    void setMovingRegionOfInterest( );
    void setReleaseRegionOfInterest( );
    void viewRegionState(int st);

     /// determina la llavor del m�ode de segmentaci�
    void setEditorPoint( );

    /// desactiva el boole�que ens diu si est�el bot�esquerra apretat
    void setLeftButtonOff( );

    /// actualitza el valor llindar baix
    void setLowerValue( int x );

     /// actualitza el valor llindar alt
    void setUpperValue( int x );

    /// Canvia la opacitat de la m�cara
    void setOpacity(int op);

    /// Canvia a la opci�esborrar
    void setErase();

    /// Canvia a la opci�pintar
    void setPaint();

    /// Canvia a la opci�esborrar llesca
    void setEraseSlice();

    /// Canvia a la opci�esborrar regió
    void setEraseRegion();

    /// Dibuixa el cursor en la forma del pinzell
    void setPaintCursor( );

    void eraseMask(int size);
    void paintMask(int size);
    void eraseSliceMask();
    void eraseRegionMask();
    void eraseRegionMaskRecursive(int a, int b, int c);

    /// Calcula el volum de la m�cara
    double calculateMaskVolume();

    /// Calcula el volum de la m�cara suposant que la variable m_cont cont�el nombre de v�els != 0 de la m�cara
    double updateMaskVolume();

    /// Refresca el resultat del volum
    void updateVolume();

    /// Visualitza la m�cara donats uns thresholds
    void viewThresholds();

    /// Visualitza els diferents overlays
    void viewLesionOverlay();

    /// Desa la màscara que s'està visualitzant
    void saveActivedMaskVolume();

    /// Desactiva les tools en cas que s'activi una tool "externa"
    void toolChanged( QAction* ac);

/*    /// sincronitza les llesques de les s?ies que es visualitzen
    void synchronizeSlices( bool ok );

    /// ens permet escollir una nova s?ie per a comparar
    void chooseNewSerie();

  signals:
    /// Aquest senyal s'emetr?quan es vulgui canviar de s?ie per comparar
    void newSerie();
*/

private:
    /// El volum principal
    Volume *m_mainVolume;

    /// El volum on hi guardem el resultat de la segmentaci�
    Volume *m_lesionMaskVolume;
    vtkImageThreshold *m_imageThreshold;

    /// El volum on hi guardem la imatge principal filtrada
    Volume *m_filteredVolume;

    /// El volum on hi guardem la fusio de la imatge i la m�cara
    //Volume *m_fusionVolume;
    //vtkImageMask *m_vtkFusionImage;

    ///Per pintar la llavor
    //vtkActor *pointActor;
    int m_seedSlice;

    /// M�ode de la segmentaci�
    rectumSegmentationMethod *m_segMethod;

    /// Membres de classe
    bool m_isSeed;
    bool m_isMask;

    bool m_isLeftButtonPressed;
    vtkActor *squareActor;
    int m_cont;
    double m_volume;

    int m_editorTool;

    vtkActor *squareRegionActor;
    double m_initialRegionPoint[2];
    double m_finalRegionPoint[2];
    bool m_isRegionSet;
    bool m_isRegionSetting;
/*    bool m_isErase;
    bool m_isPaint;
    bool m_isEraseSlice;*/


    int m_minValue, m_maxValue;
    int m_insideValue, m_outsideValue;
    int m_lowerVentriclesValue, m_upperVentriclesValue;

    /// La ToolBar de les eines de l'extensi?\TODO 'pujar' al pare com a membre com a Extensions? [hauria de ser protected]
    QToolBar *m_toolsToolBar;

    /// Accions
    QAction *m_slicingAction;
    QAction *m_windowLevelAction;
    QAction *m_zoomAction;
    QAction *m_moveAction;
    QAction *m_seedAction;
    QAction *m_editorAction;
    QAction *m_regionAction;
    QAction *m_voxelInformationAction;
    QAction *m_rotateClockWiseAction;
    ToolsActionFactory *m_actionFactory;

    QAction *m_lesionViewAction;
    QAction *m_rectumViewAction;
    QAction *m_ventriclesViewAction;

    QAction *m_paintEditorAction;
    QAction *m_eraseEditorAction;
    QAction *m_eraseSliceEditorAction;
    QAction *m_eraseRegionEditorAction;

    ///Directori on guardem les màscares
    QString m_savingMaskDirectory;

    /// Grup de botons en format exclusiu
    QActionGroup *m_toolsActionGroup;
    QActionGroup *m_viewOverlayActionGroup;
    QActionGroup* m_editorToolActionGroup;

};

} // end namespace udg

#endif
