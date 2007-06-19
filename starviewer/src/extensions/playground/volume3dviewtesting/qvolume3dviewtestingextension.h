/***************************************************************************
 *   Copyright (C) 2007 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#ifndef UDGQVOLUME3DVIEWTESTINGEXTENSION_H
#define UDGQVOLUME3DVIEWTESTINGEXTENSION_H

#include "ui_qvolume3dviewtestingextensionbase.h"

class QAction;

namespace udg {

class Volume3DViewTesting;
class Volume;
class ToolsActionFactory;

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

private:
    /// posa a disposició les tools
    void createTools();

    /// Estableix les connexions de signals i slots
    void createConnections();

private:
    /// El volum d'input
    Volume *m_input;

    /// factoria d'accions per les tools
    ToolsActionFactory *m_actionFactory;

    /// Accions de les tools
    QAction *m_zoomAction;
    QAction *m_moveAction;
    QAction *m_rotate3DAction;
    QActionGroup *m_toolsActionGroup;
};

} // end namespace udg



#endif
