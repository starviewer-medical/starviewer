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

#ifndef OPACITYTRANSFERFUNCTION_H
#define OPACITYTRANSFERFUNCTION_H

#include "transferfunctiontemplate.h"

class QVariant;
class vtkPiecewiseFunction;

namespace udg {

/**
    Representa una funció de transferència d'opacitat f: X -> O, on X és un conjunt de valors reals (valors de propietat o magnitud del gradient) i O el
    conjunt d'opacitats (reals en el rang [0,1]).
    Aquesta funció té uns quants punts definits explícitament i la resta s'obtenen per interpolació lineal o extrapolació del veí més proper.
    La funció de transferència també té un nom.
  */
class OpacityTransferFunction : public TransferFunctionTemplate<double> {

public:
    /// Crea una funció de transferència buida, sense cap punt i sense nom.
    OpacityTransferFunction();
    OpacityTransferFunction(const OpacityTransferFunction &opacityTransferFunction);
    ~OpacityTransferFunction();

    OpacityTransferFunction& operator =(const OpacityTransferFunction &opacityTransferFunction);

    /// Retorna la funció de transferència d'opacitat en format VTK.
    vtkPiecewiseFunction* vtkOpacityTransferFunction() const;

    /// Retorna la funció representada en forma d'string.
    QString toString() const;

    /// Retorna la funció representada en forma de QVariant.
    QVariant toVariant() const;
    /// Retorna la funció representada per variant.
    static OpacityTransferFunction fromVariant(const QVariant &variant);

private:
    /// Funció de transferència d'opacitat en format VTK.
    // S'ha de guardar per poder fer el Delete() més tard
    mutable vtkPiecewiseFunction *m_vtkOpacityTransferFunction;

};

} // End namespace udg

#endif // OPACITYTRANSFERFUNCTION_H
