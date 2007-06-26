/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGQMPRCARDIAC3DEXTENSION_H
#define UDGQMPRCARDIAC3DEXTENSION_H

#include "ui_qmprcardiac3dextensionbase.h"

class QBasicTimer;

namespace udg {

// FWD declarations
class Volume;
class ToolsActionFactory;

/**
Extensió de l'MPR 3D

@author Grup de Gràfics de Girona  ( GGG )
*/
class QMPRCardiac3DExtension : public QWidget , private ::Ui::QMPRCardiac3DExtensionBase {
Q_OBJECT
public:
    QMPRCardiac3DExtension( QWidget *parent = 0 );

    ~QMPRCardiac3DExtension();

    /**
        Li assigna el volum amb el que s'aplica l'MPR.
    */
    void setInput( Volume *input );

private:
    /// El volum d'entrada
    Volume* m_volume;

    /// factoria d'accions per les tools
    ToolsActionFactory *m_actionFactory;

    /// Variables de reproducció
    int m_firstSliceInterval;
    int m_lastSliceInterval;
    int m_nextStep;
    QBasicTimer *m_timer;

    /// Accions de les tools
    QAction *m_zoomAction;
    QAction *m_moveAction;
    QAction *m_rotate3DAction;
    QAction *m_screenShotAction;
    QActionGroup *m_toolsActionGroup;

    /// posa a disposició les tools
    void createTools();

    /// Estableix les connexions de signals i slots
    void createConnections();
    void timerEvent(QTimerEvent *event);
private slots:

    /// Fa la reproducció de la llesca
    void playImages();

    /// Fer la gravació
    void recordVideo();

    /// Atura la reporducció de la llesca
    void pauseImages();

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
};

};  //  end  namespace udg

#endif
