/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGQMPR3DEXTENSION_H
#define UDGQMPR3DEXTENSION_H

#include "ui_qmpr3dextensionbase.h"
using namespace Ui; // \TODO això hauria de ser temporal

namespace udg {

class Volume;
/**
Extensió de l'MPR 3D

@author Grup de Gràfics de Girona  ( GGG )
*/
class QMPR3DExtension : public QWidget , private QMPR3DExtensionBase{
Q_OBJECT
public:
    QMPR3DExtension( QWidget *parent = 0 );

    ~QMPR3DExtension();
    
    /**
        Li assigna el volum amb el que s'aplica l'MPR.
    */
    void setInput( Volume *input );

private:
    /// El volum d'entrada
    Volume* m_volume;

    /// Estableix les connexions de signals i slots
    void createConnections();

private slots:
    /// Canvia els ajustaments del window level per uns de predeterminats. 
    void changeDefaultWindowLevel( int which );
};

};  //  end  namespace udg 

#endif
