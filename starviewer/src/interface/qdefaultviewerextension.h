/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGQDEFAULTVIEWEREXTENSION_H
#define UDGQDEFAULTVIEWEREXTENSION_H

#include "ui_qdefaultviewerextensionbase.h"

namespace udg {

// FWD declarations
class Volume;

/**
Extensió que s'executarà per defecte a l'obrir un model

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class QDefaultViewerExtension : public QWidget , private Ui::QDefaultViewerExtensionBase {
Q_OBJECT
public:
    QDefaultViewerExtension( QWidget *parent = 0 );

    ~QDefaultViewerExtension();

    /// Li assigna el volum principal
    void setInput( Volume *input );

public slots:
    /// Canvia a la vista axial, sagital o coronal
    void changeViewToAxial();
    void changeViewToSagital();
    void changeViewToCoronal();

    /// Permuta l'ajustament de window level
    void changeDefaultWindowLevel( int which );
private:
    /// El volum principal
    Volume *m_mainVolume;

    /// Crea les connexions entre signals i slots
    void createConnections();
};

}

#endif
