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

#ifndef UDGQCOLORSPINBOX_H
#define UDGQCOLORSPINBOX_H

#include <QSpinBox>

namespace udg {

/**
    És un QSpinBox amb un color RGBA associat. El color RGB es visualitza com a
    color de fons de l'spinbox i l'alfa és el valor de l'spinbox (entre 0 i 255).

    \author Grup de Gràfics de Girona (GGG) <vismed@ima.udg.edu>
  */
class QColorSpinBox : public QSpinBox {
Q_OBJECT

public:
    /// Crea un QColorSpinBox blanc i amb alfa 0.
    QColorSpinBox(QWidget *parent = 0);
    virtual ~QColorSpinBox();

    /// Retorna el color RGBA associat a l'spinbox.
    const QColor& getColor() const;

public slots:
    /// Assigna un color RGBA a l'spinbox.
    void setColor(const QColor &color);

signals:
    /// S'emet quan canvia el color RGBA associat a l'spinbox.
    void colorChanged(const QColor &color);

private slots:
    /// Assigna una opacitat a l'spinbox.
    void setOpacity(int opacity);

private:
    /// Color RGBA associat a l'spinbox.
    QColor m_color;
    /// Booleà de control per evitar d'emetre dues vegades el signal colorChanged.
    bool m_settingColor;

};

}

#endif
