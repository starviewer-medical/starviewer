/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGQ2DVIEWEREXTENSION_H
#define UDGQ2DVIEWEREXTENSION_H

#include "ui_q2dviewerextensionbase.h"
//Estructura pacient
#include "patient.h"
//Visualitzador
#include "q2dviewerwidget.h"

// FWD declarations
class QAction;

namespace udg {

// FWD declarations
class Volume;
class ToolsActionFactory;
class Q2DViewerKeyImageNoteAttacher;
class Q2DViewerPresentationStateAttacher;
class KeyImageNote;

/**
Extensió que s'executarà per defecte a l'obrir un model

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class Q2DViewerExtension : public QWidget , private Ui::Q2DViewerExtensionBase {
Q_OBJECT
public:
    Q2DViewerExtension( QWidget *parent = 0 );

    ~Q2DViewerExtension();

    /// Li assigna el volum principal
    void setInput( Volume *input );

    /// Mètode per assignar un pacient
    void setPatient( Patient *patient );

    /// Mètode per obtenir el pacient
    Patient* getPatient() const;

public slots:
    /// Canvia a la vista axial, sagital o coronal
    void changeViewToAxial();
    void changeViewToSagital();
    void changeViewToCoronal();

    /// Carrega un Key Image Note
    void loadKeyImageNote(const QString &filename);

    /// Carrega un Presentation State
    void loadPresentationState(const QString &filename);

    ///Canviar el nombre de files i columnes
    void addColumns( int columns = 1 );
    void addRows(  int rows = 1 );
    void removeColumns( int columns = 1 );
    void removeRows( int rows = 1 );
    void setGrid( int rows, int columns );

    /// Mostrar menu per seleccionar grid predefinit
    ///TODO S'ha de canviar el mètode per tal que no es crei l'objecte cada cop 
    void showPredefinedGrid();

    /// Mostrar el menu de la taula per seleccionar grids
    ///TODO S'ha de canviar el mètode per tal que no es crei l'objecte cada cop
    void showInteractiveTable();

    /// Mostrar menu per seleccionar grid predefinit dins una serie
    ///TODO S'ha de canviar el mètode per tal que no es crei l'objecte cada cop
    void showPredefinedImageGrid();

    /// Mostrar el menu de la taula per seleccionar grids dins una serie
    ///TODO S'ha de canviar el mètode per tal que no es crei l'objecte cada cop
    void showInteractiveImageTable();

private slots:
    /// activem o desactivem el presentation state
    void enablePresentationState( bool enable );

    /// Inicialitza els layouts
    void initLayouts();

    /// Posem el widget seleccionat com a actual
    void setViewerSelected( Q2DViewerWidget * viewer );

    /// Slots per canviar rotacions al widget seleccionat
    void rotateClockWise();
    void rotateCounterClockWise();
    void setVoxelInformationCaptionEnabled(bool option);
    void horizontalFlip();
    void verticalFlip();
    void setWindowLevel(double wl1 ,double wl2);
    void resetWindowLevelToDefault();

private:
    /// crea les accions \TODO 'pujar' al pare com a mètode virtual comú a Extensions? [hauria de ser protected]
    void createActions();

    /// Crea les connexions entre signals i slots
    void createConnections();

    /// Llegir/Escriure la configuració de l'aplicació
    void readSettings();
    void writeSettings();

    /// Retorna un nou widget Q2DViewerWidget per poder-lo inserir a una nova fila o columna
    Q2DViewerWidget *getNewQ2DViewerWidget();

private:
    /// Tipus de vistes que podem tenir
    enum ViewType{ Axial , Sagital , Coronal };

    /// canvia la vista actual
    void setView( ViewType view );

    /// La vista actual amb la que estem treballant
    ViewType m_currentView;

    /// El volum principal
    Volume *m_mainVolume;

    /// Accions
    QAction *m_axialViewAction;
    QAction *m_sagitalViewAction;
    QAction *m_coronalViewAction;
    QAction *m_slicingAction;
    QAction *m_windowLevelAction;
    QAction *m_zoomAction;
    QAction *m_moveAction;
    QAction *m_voxelInformationAction;
    QAction *m_rotateClockWiseAction;
    QAction *m_rotateCounterClockWiseAction;
    QAction *m_flipHorizontalAction;
    QAction *m_flipVerticalAction;
    QAction *m_screenShotAction;
    QAction *m_distanceAction;
    QAction *m_roiAction;
    ToolsActionFactory *m_actionFactory;

    QAction *m_presentationStateAction;
    /// Grup de botons en format exclusiu
    QActionGroup *m_toolsActionGroup;

    /// El diàleg per escollir un window level ajustat per l'usuari
    QCustomWindowLevelDialog *m_customWindowLevelDialog;

    Q2DViewerKeyImageNoteAttacher *m_keyImageNoteAttacher;
    KeyImageNote *m_keyImageNote;

    /// S'encarrega d'aplicar els presentation states
    Q2DViewerPresentationStateAttacher *m_presentationStateAttacher;

    /// Grids per mostrar diferents q2dviewers alhora.
    QGridLayout * m_gridLayout;
    QVBoxLayout * m_verticalLayout;
    QVector<QHBoxLayout*> m_qHorizontalLayoutVector;

    /// Visualitzador, sempre en tindrem un
    Q2DViewerWidget * m_viewer;

    /// Visualitzador seleccionat, també sempre en tindrem un
    Q2DViewerWidget * m_selectedViewer;

    /// Nombre de files i columnes per els layouts
    int m_rows;
    int m_columns;

    /// Renderers que tenim
    QVector<Q2DViewerWidget *> m_vectorViewers;

    /// Pacient que estem tractant
    Patient *m_patient;
};

} // end namespace udg

#endif
