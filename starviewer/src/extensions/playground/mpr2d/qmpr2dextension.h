/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGQMPR2DEXTENSION_H
#define UDGQMPR2DEXTENSION_H

#include "ui_qmpr2dextensionbase.h"
#include <QString>

// FWD declarations
class QAction;
class QStringList;

namespace udg {

// FWD declarations
class Volume;
class ToolManager;
/**
    Extensió encarregada de fer l'MPR 2D
*/
class QMPR2DExtension : public QWidget , private Ui::QMPR2DExtensionBase {
Q_OBJECT
public:
    QMPR2DExtension( QWidget *parent = 0 );

    ~QMPR2DExtension();

public slots:
    /**
        Li assigna el volum amb el que s'aplica l'MPR.
        A cada finestra es veurà el tall corresponent per defecte. De bon principi cada visor visualitzarà la llesca central corresponent a cada vista. En les vistes axial i sagital es veuran els respectius plans de tall.
    */
    void setInput( Volume *input );

    /// Canvia la distribució horitzontal de les finestres ( el que està a la dreta passa a l'esquerra i viceversa )
    void switchHorizontalLayout();

private slots:
    ///
    /// TODO TOTS AQUESTS MÈTODES SÓN UN HACK!!!
    /// Serveixen per poder fer servir l'ImageProjectionTool amb la resta de tools
    ///
    /// Permet activar i desactivar l'eina imagePlaneProjectionTool en funció si està actiu 
    /// el botó de la tool o no
    void setEnabledImagePlaneProjectionTool(bool enable);

    /// Si una eina de botó esquerra s'ha activat desactiva l'eina d'ImageProjectionTool
    void onLeftButtonToolToggled(bool toggled);

    /// Deshabilita les eines de botó esquerre
    void disableLeftButtonTools();

private:
    ///Inicialitza les tools que tindrà l'extensió
    void initializeTools();

    /**
     * Inicialitza les tools per defecte dels visors de l'extensió
     */
    void initializeDefaultTools();

    /// Llegir/Escriure la configuració de l'aplicació
    void readSettings();
    void writeSettings();

    /// crea les connexions entre signals i slots
    void createConnections();

    /// Inicialitzador d'objectes pel constructor
    void init();

private:
    /// ToolManager per configurar l'entorn de tools de l'extensió
    ToolManager *m_toolManager;

    /// La llista de tools que són exclusives amb el botó esquerre
    QStringList m_leftButtonExclusiveTools;
};

};  //  end  namespace udg

#endif
