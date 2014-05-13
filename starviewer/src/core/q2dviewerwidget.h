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

#ifndef UDGQ2DVIEWERWIDGET_H
#define UDGQ2DVIEWERWIDGET_H

#include "ui_q2dviewerwidgetbase.h"

// FWD declarations
class QAction;

namespace udg {

class StatsWatcher;
class QFusionBalanceWidget;
class QViewerCommand;

/**
    Aquesta classe és la interfície genèrica d'un Q2DViewer juntament amb el seu slider i spinbox corresponent. Conté el mètode per posar un input així
    com els mètodes per modificar el valor de l'slider i l'spinbox.
 */
class Q2DViewerWidget : public QFrame, private Ui::Q2DViewerWidgetBase {
Q_OBJECT
public:
    Q2DViewerWidget(QWidget *parent = 0);
    ~Q2DViewerWidget();

    /// Marca el widget com a seleccionat
    void setSelected(bool option);

    /// Retorna el visualitzador
    Q2DViewer* getViewer();

    /// Retorna cert si el model té fases, fals altrament.
    bool hasPhases();

    /// Acció pel botó de sincronitzacio
    void setDefaultAction(QAction *synchronizeAction);

    void setInput(Volume *input);
    void setInputAsynchronously(Volume *input, QViewerCommand *command = 0);

    /// Habilita o deshabilita el botó que permet activar o desactivar l'eina de sincronització
    void enableSynchronizationButton(bool enable);

public slots:
    /// Habilita o deshabilita l'eina de sincronització en el visor, si aquest la té registrada
    /// Aquest mètode es podrà invocar al clicar sobré el botó de sincronització o bé cridant-lo directament
    void enableSynchronization(bool enable);

signals:
    /// Aquest senyal s'emetrà quan el mouse entri al widget
    void selected(Q2DViewerWidget *viewer);

    void manualSynchronizationStateChanged(bool enable);

protected:
    /// Sobrecàrrega de l'event que s'emet quan el mouse fa un clic dins l'àmbit del widget
    void mousePressEvent(QMouseEvent *mouseEvent);

private:
    /// Crea les connexions entre signals i slots
    void createConnections();

    /// Activa/Desactiva tots els widgets que es troben a la barra de l'slider, és a dir, tots excepte el viewer.
    void setSliderBarWidgetsEnabled(bool enabled);

private slots:
    /// Aquest slot es cridarà quan es faci alguna acció sobre l'slider
    /// i segons l'acció rebuda actualitzarà el valor de la llesca al visor
    /// Així doncs, quan l'usuari mogui l'slider, ja sigui amb la rodeta del
    /// mouse o pitjant a sobre del widget, li donarem el valor correcte al viewer
    void updateViewerSliceAccordingToSliderAction(int action);

    void updateInput(Volume *input);

    /// Fem que el viewer sigui actiu
    void setAsActiveViewer();
    
    /// Quan el visualitzador s'ha seleccionat, emet el senyal amb aquest widget
    void emitSelectedViewer();

    /// Actualitza correctament el valor de l'slider quan hi ha thick slab
    void updateSlider();

    /// Assigna el rang i el valor corresponent de l'slider segons l'input del viewer
    void resetSliderRangeAndValue();

    /// Actualitza el valor de l'etiqueta que ens indica quina
    /// projecció del pla estem veient
    void updateProjectionLabel();

    /// Actualitza l'estat d'habilitat dels widgets de la barra amb l'slider a partir de l'estat del viewer
    void setSliderBarWidgetsEnabledFromViewerStatus();

    /// If the viewer has 2 inputs shows the fusion balance tool button and resets the fusion balance to 50% for each input. Otherwise hides the button.
    void resetFusionBalance();

private:
    /// Acció del boto de sincronitzar
    QAction *m_synchronizeButtonAction;

    /// Per fer estadístiques
    StatsWatcher *m_statsWatcher;

    /// Widget to adjust the fusion balance. Will be shown as the menu of the fusion balance tool button.
    QFusionBalanceWidget *m_fusionBalanceWidget;
};

};

#endif
