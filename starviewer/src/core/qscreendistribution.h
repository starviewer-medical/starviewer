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

#ifndef UDGQSCREENDISTRIBUTION_H
#define UDGQSCREENDISTRIBUTION_H

#include <QWidget>
#include <QList>
#include <QRect>

class QPaintEvent;
class QMouseEvent;

namespace udg {

/**
    Classe que mostra les pantalles (o monitors) disponibles en una petita finestra (QDialog)
    i deixa seleccionar a quin es vol moure la finestra principal.
    Retorna l'identificador de la pantalla, i és fora d'aquesta que s'ha de fer les operacions
    de moure o no la pantalla.
  */
class QScreenDistribution : public QWidget {
Q_OBJECT

public:
    /// Constructor, window és la finestra que es mourà.
    QScreenDistribution(QWidget *parent = 0);
    /// Destructor per defecte
    ~QScreenDistribution();

signals:
    int screenClicked(int screenIndex);

protected:
    /// Retorna la mida adequada per a visualitzar correctament la finestra (400,200)
    QSize sizeHint() const;
    /// Captura del moviment del ratolí, comproba si el ratolí està a sobre d'algun dels icones de les
    /// pantalles i el repinta donant-li relleu.
    void mouseMoveEvent(QMouseEvent *event);
    /// Captura la posició del click de ratolí i comproba sobre quin icona de pantalla s'ha fet click.
    /// A continuació crida a ScreenManager per moure la finestra principal a aquesta.
    void mousePressEvent(QMouseEvent *event);
    /// Pinta la distribució de les pantalles. Si el ratolí està sobre d'alguna, la pinta amb relleu.
    void paintEvent(QPaintEvent *event);

private:
    /// Calcula el tamany de la finestra i calcula les posicions de les icones de les finestres
    /// per que quedin centrades i escalades correctament.
    void computeSizesAndPositions();

private:
    /// Llista de rectangles que representen monitors, amb posicións preparades per dibuixar.
    QList<QRect> m_screens;
    /// Indica l'identificador del monitor sobre l'icone el que està situat el ratolí.
    int m_mouseInScreen;
    /// Tamany de lletra utilitzat pel número de cada pantalla.
    int m_screenNumberPixelSize;

    /// Marge al voltant de la finestra per on començar a pintar.
    static const int WidgetMargin;
    /// Tamany màxim que pot tenir el número de cada pantalla.
    static const int MaximumScreenNumberPixelSize;
};

} // End namespace udg

#endif // UDGQSCREENDISTRIBUTION_H
