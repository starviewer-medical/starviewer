/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGQMPR3D2DEXTENSION_H
#define UDGQMPR3D2DEXTENSION_H

#include "ui_qmpr3d2dextensionbase.h"

//  FWD decs
class vtkAxisActor2D;
class QAction;

namespace udg {

// FWD declarations
class Volume;
class ToolManager;

/**
Extensió de l'MPR 3D

@author Grup de Gràfics de Girona  ( GGG )
*/
class QMPR3D2DExtension : public QWidget , private ::Ui::QMPR3D2DExtensionBase{
Q_OBJECT
public:
    QMPR3D2DExtension( QWidget *parent = 0 );

    ~QMPR3D2DExtension();

    /**
        Li assigna el volum amb el que s'aplica l'MPR.
    */
    void setInput( Volume *input );

    enum Positions{ BigView , UpperLittleView , MiddleLittleView , BottomLittleView };

public slots:
    /// permuta les posicions de les 3 vistes alineades verticalment
    void switchViews();

    /// Canvia de posició la finestra "gran" d'esquerra a dreta i vicerversa
    void switchBigView();

private slots:
    /**
     * Aquest slot es crida cada cop que es renovi l'input de l'extensió
     * actualitzant els valors de window level que es mostren per pantalla,
     * botons de l'extensió, etc
     */
    void updateExtension(Volume *volume);

    /// Actualitza les vistes 2D conforme el que es veu a la vista MPR 3D
    void update2DViews();

    /// Refresca la posició dels actors d'acord amb l'interacció
    void updateActors();

private:
    /// crea les tools
    void initializeTools();

    /// crea les accions
    void createActions();

    /// Llegir/Escriure la configuració de l'aplicació
    void readSettings();
    void writeSettings();

    /// Crea actors adicionals que s'afegiran als respectius visors
    void createActors();

    /// Estableix les connexions de signals i slots
    void createConnections();

private:
    /// El volum d'entrada
    Volume* m_volume;

    /// Accions sobre els layouts
    QAction *m_leftRightLayoutAction, *m_viewsLayoutAction;

    /// els actors que serveixen de punts de referència sobre els plans 2D
    vtkAxisActor2D *m_sagitalOverAxialIntersectionAxis , *m_coronalOverAxialIntersectionAxis , *m_axialOverSagitalIntersectionAxis , *m_coronalOverSagitalIntersectionAxis;

    /// Gestor de tools de l'extensió
    ToolManager *m_toolManager;
};

};  //  end  namespace udg

#endif
