/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGQMPR3DEXTENSION_H
#define UDGQMPR3DEXTENSION_H

#include "qmpr3dextensionbase.h"

namespace udg {

class Volume;
/**
Extensió de l'MPR 3D

@author Grup de Gràfics de Girona  ( GGG )
*/
class QMPR3DExtension : public QMPR3DExtensionBase{
Q_OBJECT
public:
    QMPR3DExtension(QWidget *parent = 0, const char *name = 0);

    ~QMPR3DExtension();
    
    /**
        Li assigna el volum amb el que s'aplica l'MPR.
    */
    void setInput( Volume *input );

private:
    /// El volum d'entrada
    Volume* m_volume;

};

};  //  end  namespace udg 

#endif
