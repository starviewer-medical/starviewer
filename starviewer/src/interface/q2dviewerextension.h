/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGQ2DVIEWEREXTENSION_H
#define UDGQ2DVIEWEREXTENSION_H

#include "ui_q2dviewerextensionbase.h"

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

public slots:
    /// Canvia a la vista axial, sagital o coronal
    void changeViewToAxial();
    void changeViewToSagital();
    void changeViewToCoronal();

    /// Canvia el layout de visor singular/doble
    void changeViewToSingle();
    void changeViewToDouble();

    /// Li assigna el volum secundari. Aquest mètode només és de conveniència i és temporal
    void setSecondInput( Volume *input );

    /// Carrega un Key Image Note
    void loadKeyImageNote(const QString &filename);

    /// Carrega un Presentation State
    void loadPresentationState(const QString &filename);

private:
    /// Tipus de vistes que podem tenir
    enum ViewType{ Axial , Sagital , Coronal };

    /// canvia la vista actual
    void setView( ViewType view );

    /// La vista actual amb la que estem treballant
    ViewType m_currentView;

    /// El volum principal
    Volume *m_mainVolume;

    /// Membre temporal
    Volume *m_secondaryVolume;

    /// Accions
    QAction *m_axialViewAction;
    QAction *m_sagitalViewAction;
    QAction *m_coronalViewAction;
    QAction *m_singleViewAction;
    QAction *m_doubleViewAction;
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
    ToolsActionFactory *m_actionFactory;

    QAction *m_presentationStateAction;
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

    Q2DViewerKeyImageNoteAttacher *m_keyImageNoteAttacher1, *m_keyImageNoteAttacher2;
    KeyImageNote *m_keyImageNote;

    /// S'encarrega d'aplicar els presentation states
    Q2DViewerPresentationStateAttacher *m_presentationStateAttacher;

private slots:
    /// sincronitza les llesques de les sèries que es visualitzen
    void synchronizeSlices( bool ok );

    /// ens permet escollir una nova sèrie per a comparar
    void chooseNewSerie();

    /// activem o desactivem el presentation state
    void enablePresentationState( bool enable );

signals:
    /// Aquest senyal s'emetrà quan es vulgui canviar de sèrie per comparar
    void newSerie();
};

} // end namespace udg

#endif
