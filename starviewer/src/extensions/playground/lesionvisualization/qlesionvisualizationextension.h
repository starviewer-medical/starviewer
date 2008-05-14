/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGQRECTUMLESIONVISUALIZATION_H
#define UDGQRECTUMLESIONVISUALIZATION_H

#include "ui_qrectumlesionvisualizationextensionbase.h"

#include <QString>

// FWD declarations
class QAction;
class QToolBar;
class vtkImageMask;
class vtkImageThreshold;
class vtkActor;
class vtkActorCollection;
class vtkPropPicker;

namespace udg {

// FWD declarations
class Volume;
class rectumSegmentationMethod;
class ToolsActionFactory;
class ToolManager;

/**
    @author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class QRectumLesionVisualizationExtension : public QWidget , private ::Ui::QRectumLesionVisualizationExtensionBase {
Q_OBJECT
public:
    QRectumLesionVisualizationExtension( QWidget *parent = 0 );
    ~QRectumLesionVisualizationExtension();

public slots:
    /// Li assigna el volum principal
    void setInput( Volume *input );

private:

    void createActions();

    /// Crea les connexions entre signals i slots
    void createConnections();

    /// Guarda i llegeix les caracter�tiques
    void readSettings();
    void writeSettings();
    
    void createCylinder();

    void extractLesionMaskArea();
    
    void mouseEvent();
    
    void setSlice();
    
private slots:
     /// gestiona els events
    void eventHandler( unsigned long id );

    /// Canvia la opacitat de la màscara
    void setOpacity(int op);
    
    /// obre la màscara de lesió
    void openMaskVolume();
    
    void markCurrentCylinder( int slice );

private:
    /// El volum principal
    Volume *m_mainVolume;

    /// El volum on hi guardem la lesió
    Volume *m_lesionMaskVolume;

    /// Accions
    QAction *m_slicingAction;
    QAction *m_windowLevelAction;
    QAction *m_zoomAction;
    QAction *m_moveAction;
    ToolsActionFactory *m_actionFactory;

    ///Directori on guardem les màscares
    QString m_defaultOpenDirectory;

    /// Grup de botons en format exclusiu
    QActionGroup *m_toolsActionGroup;
    QActionGroup *m_viewOverlayActionGroup;

    /// Tool Manager
    ToolManager *m_toolManager;
    
    ///nombre de llesques
    int m_numberOfSlices;
    
    int *m_maskLesionSlicesArea;
    
    vtkActorCollection *m_cylindersCollection;
    
    bool m_inside3Dviewer;
    
    int m_maxLesion;
    
    vtkPropPicker *picker;
};

} // end namespace udg

#endif
