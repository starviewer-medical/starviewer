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

namespace udg {

// FWD declarations
class Volume;
class ToolManager;
class QViewerCINEController;

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

private slots:
    ///Genera un fitxer de video
    void recordVideo();

    /// Fixa l'inici de l'interval de reproducció
    void initInterval( bool checked );

    /// Fixa el final de l'interval de reproducció
    void finishInterval( bool checked );

private:
    /// Tipus de vistes que podem tenir
    enum ViewType{ Axial , Sagital , Coronal };

    /// canvia la vista actual
    void setView( ViewType view );

    /// crea les accions \TODO 'pujar' al pare com a mètode virtual comú a Extensions? [hauria de ser protected]
    void createActions();

    /// Crea les connexions entre signals i slots
    void createConnections();

    /// Inicialitza les tools de l'extensió
    void initializeTools();

private:
    /// La vista actual amb la que estem treballant
    ViewType m_currentView;

    /// El volum principal
    Volume *m_mainVolume;

    /// Variables de reproducció
    int m_firstSliceInterval;
    int m_lastSliceInterval;

    /// Accions
    QAction *m_axialViewAction;
    QAction *m_sagitalViewAction;
    QAction *m_coronalViewAction;
    QAction *m_rotateClockWiseAction;
    QAction *m_rotateCounterClockWiseAction;
    QAction *m_playAction;
    QAction *m_recordAction;
    QAction *m_boomerangAction;
    QAction *m_repeatAction;
    QAction *m_sequenceBeginAction;
    QAction *m_sequenceEndAction;

    /// El diàleg per escollir un window level ajustat per l'usuari
    QCustomWindowLevelDialog *m_customWindowLevelDialog;

    /// Gestor de tools de l'extensió
    ToolManager *m_toolManager;

    /// Controlador de CINE
    QViewerCINEController *m_viewerCineController;
};

} // end namespace udg

#endif
