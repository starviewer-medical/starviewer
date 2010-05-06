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

@author Grup de Gràfics de Girona  ( GGG )

\TODO Afegir l'interacció de l'usuari. Per rotar i traslladar els plans podem seguir com a model el que fan els mètodes vtkImagePlaneWidget::Spin() i vtkImagePlaneWidget::Translate()
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

    /// Crea les accions
    void createActions();

    /// crea les connexions entre signals i slots
    void createConnections();

    /// Inicialitzador d'objectes pel constructor
    void init();

private:
    /// Ens serà molt útil ens molts de càlculs i a més serà una dada constant un cop tenim l'input
    double m_axialSpacing[3];

    /// aquesta variable ens servirà per controlar on col·loquem la llesca del pla axial
    double m_axialZeroSliceCoordinate;

    /// Filtre de fitxers que es poden desar
    QString m_fileSaveFilter;

    ///conjunt de tools disponibles en aquesta extensió
    QStringList m_extensionToolsList;

    /// Acció per poder controlar el layout horizontal
    QAction *m_horizontalLayoutAction;

    /// ToolManager per configurar l'entorn de tools de l'extensió
    ToolManager *m_toolManager;

};

};  //  end  namespace udg

#endif
