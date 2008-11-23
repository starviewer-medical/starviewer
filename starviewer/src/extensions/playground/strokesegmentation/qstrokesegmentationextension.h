/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGQSTROKESEGMENTATION_H
#define UDGQSTROKESEGMENTATION_H

#include "ui_qstrokesegmentationextensionbase.h"

#include <QString>

// FWD declarations
class QAction;
class QToolBar;
class vtkImageMask;
class vtkActor;

namespace udg {

// FWD declarations
class Volume;
class StrokeSegmentationMethod;
class ToolsActionFactory;
class ToolManager;

/**
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class QStrokeSegmentationExtension : public QWidget , private ::Ui::QStrokeSegmentationExtensionBase {
Q_OBJECT
public:
    QStrokeSegmentationExtension( QWidget *parent = 0 );
    ~QStrokeSegmentationExtension();

    /// Li assigna el volum principal
    void setInput( Volume *input );

public slots:
    /// Executa l'algorisme de segmentació
    void applyMethod();

private:
    /// crea les accions \TODO 'pujar' al pare com a m?ode virtual com a Extensions? [hauria de ser protected]
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
    void leftButtonEventHandler();

    /// visualitza la informació de la llavor del mètode de segmentació
    void setSeedPosition();

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

    /// Dibuixa el cursor en la forma del pinzell
    void setPaintCursor( );

    void eraseMask(int size);
    void paintMask(int size);
    void eraseSliceMask();

    /// Calcula el volum de la màscara
    double calculateMaskVolume();

    /// Calcula el volum de la màscara suposant que la variable m_cont conté el nombre de vòxels != 0 de la màscara
    double updateMaskVolume();

    /// Refresca el resultat del volum
    void updateVolume();

    /// Visualitza la màscara donats uns thresholds
    void viewThresholds();

private:
    /// El volum principal
    Volume *m_mainVolume;

    /// El volum on hi guardem el resultat de la segmentació
    Volume *m_maskVolume;

    ///Per pintar la llavor
    int m_seedSlice;

    /// Mètode de la segmentació
    StrokeSegmentationMethod *m_segMethod;

    /// Membres de classe
    bool m_isSeed;
    bool m_isMask;
    bool m_isErase;
    bool m_isPaint;
    bool m_isEraseSlice;
    bool m_isLeftButtonPressed;
    vtkActor *squareActor;
    int m_cont;
    double m_volume;

    int m_minValue, m_maxValue;
    int m_insideValue, m_outsideValue;

    /// Accions
    QAction *m_slicingAction;
    QAction *m_windowLevelAction;
    QAction *m_zoomAction;
    QAction *m_moveAction;
    QAction *m_seedAction;
    QAction *m_editorAction;
    QAction *m_rotateClockWiseAction;
    ToolsActionFactory *m_actionFactory;

    /// Grup de botons en format exclusiu
    QActionGroup *m_toolsActionGroup;

    /// Tool manager
    ToolManager *m_toolManager;
};

} // end namespace udg

#endif
