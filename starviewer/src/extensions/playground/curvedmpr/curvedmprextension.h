/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGCURVEDMPREXTENSION_H
#define UDGCURVEDMPREXTENSION_H

#include "ui_curvedmprextensionbase.h"

namespace udg {

// FWD declarations
class Volume;
class ToolManager;
/**
    Aquesta extensió integra les eines per poder realitzar un MPR Curvilini
*/
class CurvedMPRExtension : public QWidget , private ::Ui::CurvedMPRExtensionBase {
Q_OBJECT
public:
    CurvedMPRExtension( QWidget *parent = 0 );
    ~CurvedMPRExtension();

public slots:
    /// Assigna el volum amb el que s'aplica l'MPR Curvilini
    void setInput( Volume *input );

private:
    /// Inicialitza les tools que tindrà l'extensió
    void initializeTools();

private:
    /// El volum al que se li practica l'MPR Curvilini
    Volume *m_volume;

    /// ToolManager per configurar l'entorn de tools de l'extensió
    ToolManager *m_toolManager;
};

};  //  end  namespace udg

#endif
