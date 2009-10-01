/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGDTI_H
#define UDGDTI_H

#include "ui_dtiextensionbase.h"

namespace udg {

// FWD declarations
class ToolManager;
class Q2DViewer;
class Volume;

/**
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class DTIExtension : public QWidget , private ::Ui::DTIExtensionBase {
Q_OBJECT
public:
    DTIExtension( QWidget *parent = 0 );
    ~DTIExtension();

public slots:
    /// Li assigna el volum principal
    void setInput( Volume *input );

private:
    /// Inicialitza les eines de l'extensió
    void initializeTools();

    /// Crea les connexions entre signals i slots
    void createConnections();

    void readDTIData( QString fileName );

private slots:


private:
    /// ToolManager per gestionar tools
    ToolManager *m_toolManager;

    /// El volum principal
    Volume *m_mainVolume;
};

} // end namespace udg

#endif
