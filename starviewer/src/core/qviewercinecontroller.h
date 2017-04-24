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

#ifndef UDGQVIEWERCINECONTROLLER_H
#define UDGQVIEWERCINECONTROLLER_H

#include <QObject>

class QAction;
class QBasicTimer;

namespace udg {

class QViewer;
class Q2DViewer;
class Volume;

/**
    Controlador de seqüències de CINE acoplable a qualsevol QViewer
  */
class QViewerCINEController : public QObject {
Q_OBJECT
public:
    QViewerCINEController(QObject *parent = 0);

    ~QViewerCINEController();

    /// Li assignem el viewer amb el que treballar
    /// @param viewer
    void setInputViewer(QViewer *viewer);

    enum CINEDimensionType { SpatialDimension, TemporalDimension };
    void setCINEDimension(int dimension);

    QAction* getPlayAction() const;
    QAction* getLoopAction() const;
    QAction* getBoomerangAction() const;

signals:
    void playing();
    void paused();
    void velocityChanged(int velocity);

public slots:
    /// Engega la reproducció
    void play();

    /// Pausa/para la reproducció
    void pause();

    /// Li donem la velocitat de reproducció expresada en el nombre d'imatges que volem veure per segon
    void setVelocity(int imagesPerSecond);

    /// L'indiquem que volem engegar el mode loop
    /// En aquest mode, un cop arribem a la última imatge, tornem a començar de nou des de la inicial
    void enableLoop(bool enable);

    /// L'indiquem que volem engegar el mode boomerang
    /// En aquest mode es recorren les imatges repetidament en l'ordre 1..n n..1 (endavant i endarrera)
    void enableBoomerang(bool enable);

    /// Li indiquem l'interval de reproducció
    void setPlayInterval(int firstImage, int lastImage);

    /// Reseteja la informació de CINE segons l'input donat
    /// @param input
    void resetCINEInformation(Volume *input);

    /// Actualitza la informació que té del volum actual
    void updateSliceRange();

protected:
    void timerEvent(QTimerEvent *event);

private:
    /// Aquí ens ocupem de decidir cap on va el següent frame
    /// durant la reproducció
    void handleCINETimerEvent();

private:
    /// Variables de reproducció
    int m_firstSliceInterval;
    int m_lastSliceInterval;

    /// Ens indica si la reproducció va cap endavant o cap endarera
    int m_nextStep;

    /// Velocitat expresada en imatges per segon
    int m_velocity;

    QBasicTimer *m_timer;

    Q2DViewer *m_2DViewer;

    /// Indica si s'està reproduint o no
    bool m_playing;

    /// Dimensió sobre la qual es reproduirà
    int m_cineDimension;

    /// Control de loop
    bool m_loopEnabled;
    bool m_boomerangEnabled;

    QAction *m_playAction;
    QAction *m_loopAction;
    QAction *m_boomerangAction;

};

}

#endif
