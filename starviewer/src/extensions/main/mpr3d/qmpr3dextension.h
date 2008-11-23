/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGQMPR3DEXTENSION_H
#define UDGQMPR3DEXTENSION_H

#include "ui_qmpr3dextensionbase.h"

namespace udg {

// FWD declarations
class Volume;
class ToolManager;

/**
Extensió de l'MPR 3D

@author Grup de Gràfics de Girona  ( GGG )
*/
class QMPR3DExtension : public QWidget , private ::Ui::QMPR3DExtensionBase{
Q_OBJECT
public:
    QMPR3DExtension( QWidget *parent = 0 );

    ~QMPR3DExtension();

    /**
        Li assigna el volum amb el que s'aplica l'MPR.
    */
    void setInput( Volume *input );

private slots:
    /**
     * Aquest slot es crida cada cop que es renovi l'input de l'extensió
     * actualitzant els valors de window level que es mostren per pantalla,
     * botons de l'extensió, etc
     */
    void updateExtension( Volume *volume );

private:
    /// posa a disposició les tools
    void initializeTools();

    /// Estableix les connexions de signals i slots
    void createConnections();

private:
    /// El volum d'entrada
    Volume* m_volume;

    /// Gestor de tools de l'extensió
    ToolManager *m_toolManager;
};

};  //  end  namespace udg

#endif
