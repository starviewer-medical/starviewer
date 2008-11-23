/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGQ2DVIEWEREXTENSION_H
#define UDGQ2DVIEWEREXTENSION_H

#include "ui_q2dviewerextensionbase.h"
#include <QWidget>

// FWD declarations
class QAction;

namespace udg {

// FWD declarations
class Volume;
class ToolsActionFactory;
class Q2DViewerKeyImageNoteAttacher;
class Q2DViewerPresentationStateAttacher;
class KeyImageNote;
class ToolManager;
class ToolConfiguration;
class HangingProtocol;
//Estructura pacient
class Patient;
//Visualitzador
class Q2DViewerWidget;
class Q2DViewer;
// Menus
class MenuGridWidget;
class TableMenu;
class QDicomDump;

/**
Extensió que s'executarà  per defecte a l'obrir un model

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
    void resetViewToAxial();
    void resetViewToSagital();
    void resetViewToCoronal();

    /// Carrega un Key Image Note
    void loadKeyImageNote(const QString &filename);

    /// Carrega un Presentation State
    void loadPresentationState(const QString &filename);

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

    /// Mostrar la informació del volum a cada visualitzador o no.
    void showViewerInformation( bool show );

    ///Mostrar la pantalla de Dicom dump, amb informació de la imatge que es visualitza en aquell moment
    void showDicomDumpCurrentDisplayedImage();

    //TODO mètode per solucionar problemes perquè la sincronització encara no està adaptada
    // a la resta de les tools
    /// Desactiva l'eina de sincronització.
    void disableSynchronization();

	// Apilicar un hanging protocol
	void setHangingProtocol( int hangingProtocolNumber );

private:
    /// crea les accions \TODO 'pujar' al pare com a mètode virtual comú a Extensions? [hauria de ser protected]
    void createActions();

    /// Crea les connexions entre signals i slots
    void createConnections();

    /// Llegir/Escriure la configuració de l'aplicació
    void readSettings();
    void writeSettings();

    /**
     * Inicialitza les tools que tindrà l'extensió
     */
    void initializeTools();

    /**
     * Inicialitza les tools per defecte per a un viewer determinat
     * @param viewer viewer pel qual configurem les tools per defecte
     */
    void initializeDefaultTools( Q2DViewer *viewer );

private slots:
    /// activem o desactivem el presentation state
    void enablePresentationState( bool enable );

    /// Slots per canviar rotacions al widget seleccionat
    void rotateClockWise();
    void rotateCounterClockWise();
    void horizontalFlip();
    void verticalFlip();

    /// Comprova si el nou volum té fases i per tant hem d'activar/descativar la vista coronal+sagital
    void validePhases();

    /**
     * Habilita o deshabilita el botó de Dump d'info DICOM
     * segons en la vista en la que ens trobem
     */
    void updateDICOMInformationButton( int view );

    /// Activa la sincronització al visualitzador
    void synchronization( Q2DViewerWidget *, bool );

    /// Mètode que tracta el nou visualitzador afegit
    void activateNewViewer( Q2DViewerWidget * newViewerWidget );

    /// Mètode que tracta el canvi de visualitzador seleccionat
    void changeSelectedViewer( Q2DViewerWidget * viewerWidget );

private:
    /// El volum principal
    Volume *m_mainVolume;

    /// Accions
    QAction *m_axialViewAction;
    QAction *m_sagitalViewAction;
    QAction *m_coronalViewAction;
    QAction *m_rotateClockWiseAction;
    QAction *m_rotateCounterClockWiseAction;
    QAction *m_flipHorizontalAction;
    QAction *m_flipVerticalAction;
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

    ///Obre la finestra de dicomdump per la imatge que tenim seleccionada en aquests moments
    QDicomDump *m_dicomDumpCurrentDisplayedImage;

    /// Pacient que estem tractant
    Patient *m_patient;

    /// Menús per seleccionar el grid de series( predefinits + taula )
    MenuGridWidget *m_predefinedSeriesGrid;
    TableMenu *m_seriesTableGrid;

    /// Menús per seleccionar el grid de les llesques d'una serie ( predefinits + taula )
    MenuGridWidget *m_predefinedSlicesGrid;
    TableMenu *m_sliceTableGrid;

    /// ToolManager per configurar l'entorn deToolConfiguration * configuration = getConfiguration();
    ToolManager *m_toolManager;

    ///Perfil a mostrar la interficie
    QString m_profile;

    /// Visualitzador actual seleccionat
    Q2DViewerWidget * m_viewer;

	/// Llista de hanging protocols aplicables
	QList<HangingProtocol * > m_hangingCandidates;

};

} // end namespace udg

#endif
