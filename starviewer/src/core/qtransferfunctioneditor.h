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

#ifndef UDGQTRANSFERFUNCTIONEDITOR_H
#define UDGQTRANSFERFUNCTIONEDITOR_H

#include <QWidget>

namespace udg {

class TransferFunction;

/**
    Classe base per a qualsevol editor de funcions de transferència.

    Declara mètodes per assignar i obtenir la funció de transferència.

    \author Grup de Gràfics de Girona (GGG) <vismed@ima.udg.edu>
  */
class QTransferFunctionEditor : public QWidget {
Q_OBJECT

public:
    QTransferFunctionEditor(QWidget *parent = 0);
    virtual ~QTransferFunctionEditor();

    /// Retorna el valor de propietat màxim mapat per aquest editor.
    virtual unsigned short maximum() const;
    /// Assigna el valor de propietat màxim mapat per aquest editor.
    virtual void setMaximum(unsigned short maximum);

    /// Assigna una funció de transferència a l'editor.
    virtual void setTransferFunction(const TransferFunction &transferFunction) = 0;
    /// Retorna la funció de transferència de l'editor.
    virtual const TransferFunction& getTransferFunction() const = 0;

protected:
    /// Valor de propietat màxim mapat per aquest editor.
    unsigned short m_maximum;

};

}

#endif
