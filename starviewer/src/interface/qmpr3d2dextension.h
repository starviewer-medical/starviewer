/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGQMPR3D2DEXTENSION_H
#define UDGQMPR3D2DEXTENSION_H

// #include "qmpr3d2dextensionbase.h"
#include "ui_qmpr3d2dextensionbase.h"
using namespace Ui; // \TODO tractar de solucionar això bé

//  FWD decs
class vtkAxisActor2D;

namespace udg {

class Volume;

/**
Extensió de l'MPR 3D

@author Grup de Gràfics de Girona  ( GGG )
*/
class QMPR3D2DExtension : public QWidget , private QMPR3D2DExtensionBase{
Q_OBJECT
public:
    QMPR3D2DExtension( QWidget *parent = 0 );

    ~QMPR3D2DExtension();
    
    /**
        Li assigna el volum amb el que s'aplica l'MPR.
    */
    void setInput( Volume *input );

private:
    /// El volum d'entrada
    Volume* m_volume;
    /// Llegir/Escriure la configuració de l'aplicació
    void readSettings();
    void writeSettings();
    
    /// els actors que serveixen de punts de referència sobre els plans 2D
    vtkAxisActor2D *m_sagitalOverAxialIntersectionAxis , *m_coronalOverAxialIntersectionAxis , *m_axialOverSagitalIntersectionAxis , *m_coronalOverSagitalIntersectionAxis;
    /// Crea actors adicionals que s'afegiran als respectius visors
    void createActors();
    
    /// Estableix les connexions de signals i slots
    void createConnections();

private slots:
    /// Actualitza les vistes 2D conforme el que es veu a la vista MPR 3D
    void update2DViews();

    /// Refresca la posició dels actors d'acord amb l'interacció
    void updateActors();

    /// Canvia els ajustaments del window level per uns de predeterminats. 
    void changeDefaultWindowLevel( int which );

};

};  //  end  namespace udg 

#endif
