/*************************************************************************************
  Copyright (C) 2014 Laboratori de Gràfics i Imatge, Universitat de Girona &
  Institut de Diagnòstic per la Imatge.
  Girona 2014. All rights reserved.
  http://starviewer.udg.edu

  This file is part of the Starviewer (Medical Imaging Software) open source project.
  It is subject to the license terms in the LICENSE file found in the top-level
  directory of this distribution and at http://starviewer.udg.edu/license. No part of
  the Starviewer (Medical Imaging Software) open source project, including this file,
  may be copied, modified, propagated, or distributed except according to the
  terms contained in the LICENSE file.
 *************************************************************************************/

#ifndef UDGQ3DORIENTATIONMARKER_H
#define UDGQ3DORIENTATIONMARKER_H

#include <QObject>

class vtkOrientationMarkerWidget;
class vtkRenderWindowInteractor;
class vtkAnnotatedCubeActor;

namespace udg {

/**
    Classe que encapsula un vtkOrienatationMarker que servirà d'element d'orientació per a visors 3D. Per funcionar només cal crear
    una instància de la classe i indicar-li el vtkInteractor. La fem heretar de QObjecte per si cal connectar-li signals i slots des d'una altre classe
  */
class Q3DOrientationMarker : public QObject {
Q_OBJECT
public:
    Q3DOrientationMarker(vtkRenderWindowInteractor *interactor, QObject *parent = 0);

    ~Q3DOrientationMarker();

    /// Li posem les etiquetes de texte que han d'anar per cada cara del cub
    void setOrientationText(QString right, QString left, QString posterior, QString anterior, QString superior, QString inferior);

public slots:
    /// Mètodes per controlar si s'habilita el widget o no
    void setEnabled(bool enable);
    void enable();
    void disable();

private:
    /// Widget per veure la orientació en 3D
    vtkOrientationMarkerWidget *m_markerWidget;

    ///
    vtkAnnotatedCubeActor *m_cubeActor;
};

}

#endif
