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

namespace udg {

class Volume3DViewTesting;
class Volume;
class ToolManager;
class QClutEditorDialog;

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
    /// Actualitza el mètode de rendering que s'aplica al volum segons el que s'ha escollit al combo box
    void updateRenderingMethodFromCombo( int index );

    /// Aplica una funció de transferència predefinida a la visualització.
    void applyPresetClut( const QString & clutName );

    /// Mostra el diàleg per editar funcions de transferència.
    void showClutEditorDialog();

    /// Aplica la funció de transferència passada a la visualització.
    void applyClut( const TransferFunction & clut );

    void manageClosedDialog();

    /// Comença a calcular les obscurances, i si ja s'estan calculant ho cancel·la.
    void computeOrCancelObscurance();
    void endComputeObscurance();

private:
    /// posa a punt les tools que es poden fer servir en l'extensió
    void initializeTools();

    /// Obté la llista de funcions de transferència predefinides.
    void loadClutPresets();

    /// Estableix les connexions de signals i slots
    void createConnections();

    /// Lectura/escriptura de configuració
    void readSettings();
    void writeSettings();

    void enableObscuranceRendering( bool on );

private slots:

    void render();

private:
    /// El volum d'input
    Volume *m_input;

    /// Gestor de tools
    ToolManager *m_toolManager;

    /// Directori de funcions de transferència predefinides.
    QDir m_clutsDir;
    /// Mapa entre noms de funcions de transferència i el nom de fitxer corresponent.
    QMap<QString, QString> m_clutNameToFileName;

    /// Editor de cluts.
    QClutEditorDialog * m_clutEditorDialog;

    /// Última clut aplicada.
    TransferFunction m_currentClut;

    /// Ens indica si en aquests moments s'estan calculant les obscurances.
    bool m_computingObscurance;
};

} // end namespace udg



#endif
