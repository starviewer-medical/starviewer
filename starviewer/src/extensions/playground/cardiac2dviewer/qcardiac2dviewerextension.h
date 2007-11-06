/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGQCARDIAC2DVIEWEREXTENSION_H
#define UDGQCARDIAC2DVIEWEREXTENSION_H

#include "ui_qcardiac2dviewerextensionbase.h"

// FWD declarations
class QAction;
class QBasicTimer;

namespace udg {

// FWD declarations
class Volume;
class ToolsActionFactory;

/**
Extensió que s'executarà per defecte a l'obrir un model

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class QCardiac2DViewerExtension : public QWidget , private Ui::QCardiac2DViewerExtensionBase {
Q_OBJECT
public:
    QCardiac2DViewerExtension( QWidget *parent = 0 );

    ~QCardiac2DViewerExtension();

public slots:

    /// Li assigna el volum principal
    void setInput( Volume *input );

    /// Canvia a la vista axial, sagital o coronal
    void changeViewToAxial();
    void changeViewToSagital();
    void changeViewToCoronal();

private:
    /// Tipus de vistes que podem tenir
    enum ViewType{ Axial , Sagital , Coronal };

    /// canvia la vista actual
    void setView( ViewType view );

    /// La vista actual amb la que estem treballant
    ViewType m_currentView;

    /// El volum principal
    Volume *m_mainVolume;

    /// Variables de reproducció
    int m_firstSliceInterval;
    int m_lastSliceInterval;
    int m_nextStep;
    QBasicTimer *m_timer;

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
    QAction *m_screenShotAction;
    QAction *m_playAction;
    QAction *m_recordAction;
    QAction *m_boomerangAction;
    QAction *m_repeatAction;
    QAction *m_sequenceBeginAction;
    QAction *m_sequenceEndAction;

    ToolsActionFactory *m_actionFactory;

    /// Grup de botons en format exclusiu
    QActionGroup *m_toolsActionGroup;

    /// crea les accions \TODO 'pujar' al pare com a mètode virtual comú a Extensions? [hauria de ser protected]
    void createActions();

    /// Crea les connexions entre signals i slots
    void createConnections();

    /// Llegir/Escriure la configuració de l'aplicació
    void readSettings();
    void writeSettings();

    /// El diàleg per escollir un window level ajustat per l'usuari
    QCustomWindowLevelDialog *m_customWindowLevelDialog;

private slots:
    /// Fa la reproducció de la llesca
    void playImages();

    /// Atura la reporducció de la llesca
    void pauseImages();

    ///Genera un fitxer de video
    void recordVideo();

    /// Canvia la velocitat de la reproducció mentres s'està reproduint
    void changeVelocity( int velocity );

    /// Canvia al mode de reproducció cíclic
    void changeToLoopMode( bool checked );

    /// Canvia al mode de reproducció de davant-endarrera
    void changeToComeBackMode( bool checked );

    /// Fixa l'inici de l'interval de reproducció
    void initInterval( bool checked );

    /// Fixa el final de l'interval de reproducció
    void finishInterval( bool checked );

protected:
    void timerEvent(QTimerEvent *event);

};

} // end namespace udg

#endif
