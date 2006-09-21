/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGQ3DORIENTATIONMARKER_H
#define UDGQ3DORIENTATIONMARKER_H

#include <QObject>

class vtkOrientationMarkerWidget;
class vtkRenderWindowInteractor;

namespace udg {

/**
Classe que encapsula un vtkOrienatationMarker que servirà d'element d'orientació per a visors 3D. Per funcionar només cal crear una instància de la classe i indicar-li el vtkInteractor. La fem heretar de QObjecte per si cal connectar-li signals i slots des d'una altre classe

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class Q3DOrientationMarker: public QObject
{
Q_OBJECT
public:
    Q3DOrientationMarker( vtkRenderWindowInteractor *interactor, QObject *parent = 0);

    ~Q3DOrientationMarker();

public slots:
    /// Mètodes per controlar si s'habilita el widget o no
    void setEnabled( bool enable );
    void enable();
    void disable();
    
private:
    /// Widget per veure la orientació en 3D
    vtkOrientationMarkerWidget *m_markerWidget;
    
};

}

#endif
