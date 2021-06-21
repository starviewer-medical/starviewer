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

#ifndef UDGQVIEWERWORKINPROGRESSWIDGET_H
#define UDGQVIEWERWORKINPROGRESSWIDGET_H

#include "ui_qviewerworkinprogresswidget.h"

class QMovie;

namespace udg {

/**
    Widget que s'encarrega de visualitzar el progrés d'alguna tasca que hi pugui haver en un QViewer.
    Per exemple, el progrés de la càrrega d'un volum asíncrona, descàrrega de prèvies...
    En el cas que alguna d'aquestes tasques doni error, dona l'opció de mostrar aquests errors en el mateix
    widget.
  */

class QViewerWorkInProgressWidget : public QWidget, private ::Ui::QViewerWorkInProgressWidget {
Q_OBJECT
public:
    explicit QViewerWorkInProgressWidget(QWidget *parent = 0);

    /// Especifica quin és el títol que es posa a la tasca que s'estigui fent
    void setTitle(const QString &text);

    /// Mostra l'error que se li passi. Útil per mostrar a l'usuari si es produeix un error al fer el "work in progress".
    void showError(const QString &errorText);

    /// Reinicia el widget.
    void reset();

public slots:
    /// Actualitza el progrés del "work in progress".
    void updateProgress(int progress);

protected:
    /// Per evitar que l'animació consumeixi recursos quan no és necessita, només l'activem quan el widget és visible,
    /// desactivant-la si no ho és.
    virtual void showEvent(QShowEvent *event);
    virtual void hideEvent(QHideEvent *event);

private:
    /// Reseteja els widgets involucrats en mostrar el progrés
    void resetProgressWidgets();

private:
    QMovie *m_progressBarAnimation;
};

} // End namespace udg

#endif // UDGQVIEWERWORKINPROGRESSWIDGET_H
