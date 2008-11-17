/***************************************************************************
 *   Copyright (C) 2007 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#ifndef UDGQVOLUME3DVIEWTESTINGEXTENSION_H
#define UDGQVOLUME3DVIEWTESTINGEXTENSION_H

#include "ui_qvolume3dviewtestingextensionbase.h"

#include <QDir>
#include <QMap>

#include "transferfunction.h"

class QAction;
class QModelIndex;
class QStandardItemModel;

namespace udg {

class Volume3DViewTesting;
class Volume;
class ToolManager;

/**
 * ...
 *
 * @author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class QVolume3DViewTestingExtension : public QWidget, private ::Ui::QVolume3DViewTestingExtensionBase {
Q_OBJECT
public:

    QVolume3DViewTestingExtension( QWidget * parent = 0 );
    virtual ~QVolume3DViewTestingExtension();

    /// Li donem l'input a processar
    void setInput( Volume * input );

public slots:

    /// Aplica una funció de transferència predefinida a la visualització.
    void applyPresetClut( const QString & clutName );

    /// Aplica la funció de transferència passada a la visualització.
    void applyClut( const TransferFunction & clut, bool preset = false );

    /// Comença a calcular les obscurances, i si ja s'estan calculant ho cancel·la.
    void computeOrCancelObscurance();
    void endComputeObscurance();

    void setScalarRange( double min, double max );

    void updateUiForRenderingMethod( int index );

    /// Actualitza la vista d'acord amb tots els paràmetres actuals de la interfície.
    void updateView();

signals:

    void newTransferFunction();

private:
    /// posa a punt les tools que es poden fer servir en l'extensió
    void initializeTools();

    /// Obté la llista de funcions de transferència predefinides.
    void loadClutPresets();

    /// Carrega els estils de rendering predefinits.
    void loadRenderingStyles();

    /// Estableix les connexions de signals i slots
    void createConnections();

    /// Lectura/escriptura de configuració
    void readSettings();
    void writeSettings();

    void enableObscuranceRendering( bool on );

    /// Fa que es cridi updateView() quan canvia qualsevol element de la interfície.
    void enableAutoUpdate();
    /// Fa que no es cridi updateView() quan canvia qualsevol element de la interfície.
    void disableAutoUpdate();

private slots:

    void render();
    void autoCancelObscurance();
    void loadClut();
    void saveClut();
    void switchEditor();
    void applyEditorClut();
    void toggleClutEditor();
    void hideClutEditor();
    /// Canvia l'estat del botó del custom style depenent de si s'està mostrant o no l'editor en aquell moment.
    void setCustomStyleButtonStateBySplitter();
    void changeViewerTransferFunction();
    void applyRenderingStyle( const QModelIndex &index );

private:
    /// El volum d'input
    Volume *m_input;

    /// Gestor de tools
    ToolManager *m_toolManager;

    /// Directori de funcions de transferència predefinides.
    QDir m_clutsDir;
    /// Mapa entre noms de funcions de transferència i el nom de fitxer corresponent.
    QMap<QString, QString> m_clutNameToFileName;

    /// Última clut aplicada.
    TransferFunction m_currentClut;

    /// Ens indica si en aquests moments s'estan calculant les obscurances.
    bool m_computingObscurance;

    /// Serà cert abans d'entrar el primer input.
    bool m_firstInput;

    /// Model que guarda els estils de rendering.
    QStandardItemModel *m_renderingStyleModel;
};

} // end namespace udg



#endif
