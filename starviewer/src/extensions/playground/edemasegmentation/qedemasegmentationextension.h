/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGQEDEMASEGMENTATION_H
#define UDGQEDEMASEGMENTATION_H

#include "ui_qedemasegmentationextensionbase.h"


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
class StrokeSegmentationMethod;
class ToolsActionFactory;
class ToolManager;

/**
Extensió que segmenta l'hematoma i l'edema

    @author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/

class QEdemaSegmentationExtension : public QWidget , private ::Ui::QEdemaSegmentationExtensionBase {
Q_OBJECT
public:
    QEdemaSegmentationExtension( QWidget *parent = 0 );

    ~QEdemaSegmentationExtension();

public slots:
    /// Li assigna el volum principal
    /// És slot pq també ho fem servir en cas de canvi de sèrie
    void setInput( Volume *input );

    /// Executa l'algorisme de segmetnació
    void applyMethod();
    void applyCleanSkullMethod( );
    void applyEdemaMethod( );
    void applyVentriclesMethod( );
    void applyFilterMainImage( );
private:
    /// crea les accions \TODO 'pujar' al pare com a mètode virtual com a Extensions? [hauria de ser protected]
    void createActions();

    /// Crea les connexions entre signals i slots
    void createConnections();

    /// Guarda i llegeix les característiques
    void readSettings();
    void writeSettings();

private slots:
     /// gestiona els events del m_2DView
    void strokeEventHandler( unsigned long id );

    /// gestiona els events del botó esquerre
    void leftButtonEventHandler( );

    /// visualitza la informació de la llavor del mètode de segmentació
    void setSeedPosition( );

     /// determina la llavor del mètode de segmentació
    void setEditorPoint( );

    /// desactiva el booleà que ens diu si està el botó esquerra apretat
    void setLeftButtonOff( );

    /// actualitza el valor llindar baix
    void setLowerValue( int x );

     /// actualitza el valor llindar alt
    void setUpperValue( int x );

    /// Canvia la opacitat de la màscara
    void setOpacity(int op);

    /// Canvia a la opció esborrar
    void setErase();

    /// Canvia a la opció pintar
    void setPaint();

    /// Canvia a la opció esborrar llesca
    void setEraseSlice();

    /// Canvia a la opció esborrar regió
    void setEraseRegion();

    /// Dibuixa el cursor en la forma del pinzell
    void setPaintCursor( );

    void eraseMask(int size);
    void paintMask(int size);
    void eraseSliceMask();
    void eraseRegionMask();
    void eraseRegionMaskRecursive(int a, int b, int c);

    /// Calcula el volum de la màscara
    double calculateMaskVolume();

    /// Calcula el volum de la màscara suposant que la variable m_cont conté el nombre de vòxels != 0 de la màscara
    double updateMaskVolume();

    /// Refresca el resultat del volum
    void updateVolume();

    /// Visualitza la màscara donats uns thresholds
    void viewThresholds();

    /// Visualitza els diferents overlays
    void viewLesionOverlay();
    void viewEdemaOverlay();
    void viewVentriclesOverlay();

    /// Desa la màscara que s'està visualitzant
    void saveActivedMaskVolume();

private:
    /// El volum principal
    Volume *m_mainVolume;

    /// El volum on hi guardem el resultat de la segmentació
    Volume *m_lesionMaskVolume;
    Volume *m_edemaMaskVolume;
    Volume *m_ventriclesMaskVolume;
    Volume *m_activedMaskVolume;
    vtkImageThreshold *m_imageThreshold;

    /// El volum on hi guardem la imatge principal filtrada
    Volume *m_filteredVolume;

    ///Per pintar la llavor
    int m_seedSlice;

    /// Mètode de la segmentació
    StrokeSegmentationMethod *m_segMethod;

    /// Membres de classe
    bool m_isSeed;
    bool m_isMask;

    bool m_isLeftButtonPressed;
    vtkActor *squareActor;
    int m_cont;
    int m_edemaCont;
    int m_ventriclesCont;
    int* m_activedCont;
    double m_volume;
    double m_edemaVolume;
    double m_ventriclesVolume;
    double* m_activedVolume;

    /// tipus d'edició dels models
    enum EditorType{ NoEditor , Paint , Erase , EraseSlice , EraseRegion };
    int m_editorTool;

    int m_minValue, m_maxValue;
    int m_insideValue, m_outsideValue;
    int m_lowerVentriclesValue, m_upperVentriclesValue;

    /// Accions
    QAction *m_slicingAction;
    QAction *m_windowLevelAction;
    QAction *m_zoomAction;
    QAction *m_moveAction;
    QAction *m_seedAction;
    QAction *m_editorAction;
    QAction *m_rotateClockWiseAction;
    ToolsActionFactory *m_actionFactory;

    QAction *m_lesionViewAction;
    QAction *m_edemaViewAction;
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

    /// ToolManager
    ToolManager *m_toolManager;
};

} // end namespace udg

#endif
