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

#ifndef UDGQTRANSFERFUNCTIONEDITORBYVALUES_H
#define UDGQTRANSFERFUNCTIONEDITORBYVALUES_H

#include "qtransferfunctioneditor.h"
#include "ui_qtransferfunctioneditorbyvaluesbase.h"

#include "transferfunction.h"

namespace udg {

class QTransferFunctionIntervalEditor;

/**
    Editor de funcions de transferència basat en valors.

    L'usuari pot definir punts o intervals amb el valor RGBA que els correspon. Aquests punts o
    intervals són sempre disjunts i n'hi pot haver un màxim de <i>n</i> + 1, on <i>n</i> és el valor
    de propietat màxim del volum. Els valors de propietat són enters en l'interval [0,<i>n</i>] (el
    primer sempre comença a 0 i l'últim sempre acaba a <i>n</i>). El color es pot triar en un diàleg
    de selecció de color i l'opacitat en el mateix diàleg o directament modificant el valor d'un
    SpinBox.

    \author Grup de Gràfics de Girona (GGG) <vismed@ima.udg.edu>
  */
class QTransferFunctionEditorByValues : public QTransferFunctionEditor, private ::Ui::QTransferFunctionEditorByValuesBase {
Q_OBJECT

public:
    /// Crea l'editor amb un sol interval de 0 a 255 amb RGBA = (0,0,0,0).
    explicit QTransferFunctionEditorByValues(QWidget *parent = nullptr);
    ~QTransferFunctionEditorByValues() override;

    /// Sets the minimum input value mapped by this editor.
    void setMinimum(int minimum) override;
    /// Sets the minimum input value mapped by this editor.
    void setMaximum(int maximum) override;

    /// Assigna una funció de transferència a l'editor.
    void setTransferFunction(const TransferFunction &transferFunction) override;
    /// Retorna la funció de transferència de l'editor.
    const TransferFunction& getTransferFunction() const override;

public slots:
    /// Afegeix un interval a l'editor (al final). Si ja n'hi ha el màxim no fa res.
    void addInterval();
    /// Esborra un interval de l'editor (l'últim). Si n'hi ha només 1 no fa res.
    void removeInterval();

private:
    /// Afegeix un interval al final i el retorna. Si ja n'hi ha el màxim no fa res.
    QTransferFunctionIntervalEditor* addIntervalAndReturnIt();

private slots:
    /// Posa m_changed a cert.
    void markAsChanged();

private:
    /// Widget on es situaran els editors d'interval, dins d'una àrea d'scroll.
    QWidget *m_intervalEditorsWidget;
    /// El layout del widget anterior.
    QVBoxLayout *m_intervalEditorsLayout;

    /// Nombre d'intervals de l'editor.
    unsigned int m_numberOfIntervals;

    /// Memoria cau de la funció de transferència representada actualment per l'editor (si m_changed
    /// és fals).
    mutable TransferFunction m_transferFunction;

    /// Indica si hi ha hagut canvis des de l'últim cop que s'ha actualitzat la memòria cau de la
    /// funció de transferència.
    mutable bool m_changed;

};

}

#endif
