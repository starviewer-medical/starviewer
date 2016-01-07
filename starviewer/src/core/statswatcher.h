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

/**
 Observador de widgets per obtenir estadístiques
 TODO cal afegir un setting que ens digui si es permeten les estadístiques o no
 i en funció del seu valor registrar o no les estadístques
*/

#ifndef UDGSTATSWATCHER_H
#define UDGSTATSWATCHER_H

#include <QObject>

// Fwd declarations
class QAbstractButton;
class QAbstractSlider;
class QAction;
class QMenu;

namespace udg {
class StatsWatcher : public QObject {
Q_OBJECT
public:
    StatsWatcher(QObject *parent = 0);
    StatsWatcher(const QString &context, QObject *parent = 0);
    ~StatsWatcher();

    /// Comptador de clicks. Per cada click del botó ens dirà el nom de l'objecte
    void addClicksCounter(QAbstractButton *button);

    /// Compta quan una acció s'ha disparat
    // Compta quan es dispara, ja sigui amb un clik o un shortcut
    void addTriggerCounter(QMenu *menu);

    /// Registra les accions fetes sobre un slider
    void addSliderObserver(QAbstractSlider *slider);

    /// Mètode per loggejar missatges estadístics en el format corresponent
    static void log(const QString &message);

private slots:
    /// Registra en el log l'objecte sobre el qual s'ha fet el click
    /// Es comprova si l'objecte és "checkable" (tipus QAbstractButton)
    /// Segons això es reguistra un missatge diferent per poder diferenciar
    /// objectes únicament clickables i objectes que es poden activar o desactivar
    void registerClick(bool checked);

    /// Registra l'activació (trigger) d'una QAction
    void registerActionTrigger(QAction *action);

    /// Registra l'acció feta sobre un slider
    void registerSliderAction(int action = 10);

private:
    /// Afegeix informació adicional sobre el contexte que estem fent l'observació
    QString m_context;

    /// Indica si els logs d'estadístiques es registraran o no
    bool m_registerLogs;
};

} // End namespace udg

#endif
