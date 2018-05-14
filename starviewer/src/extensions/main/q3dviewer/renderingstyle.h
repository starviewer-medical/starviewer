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

#ifndef UDGRENDERINGSTYLE_H
#define UDGRENDERINGSTYLE_H

#include "transferfunction.h"

namespace udg {

/**
    Classe que encapsula tots els paràmetres de rendering.
  */
class RenderingStyle {

public:

    /// Mètode de rendering.
    enum Method { RayCasting, MIP, IsoSurface };

    /// Assignar/obtenir el mètode de rendering.
    Method getMethod() const;
    void setMethod(Method method);
    /// Assignar/obtenir si s'aplica il·luminació o no.
    bool getShading() const;
    void setShading(bool shading);
    /// Assignar/obtenir els paràmetres d'il·luminació.
    double getAmbientCoefficient() const;
    void setAmbientCoefficient(double ambientCoefficient);
    double getDiffuseCoefficient() const;
    void setDiffuseCoefficient(double diffuseCoefficient);
    double getSpecularCoefficient() const;
    void setSpecularCoefficient(double specularCoefficient);
    double getSpecularPower() const;
    void setSpecularPower(double specularPower);
    /// Assignar/obtenir la funció de transferència.
    const TransferFunction& getTransferFunction() const;
    void setTransferFunction(const TransferFunction &transferFunction);
    /// Assignar/obtenir l'iso-valor per les iso-superfícies.
    double getIsoValue() const;
    void setIsoValue(double isoValue);

    /// Mètodes per convertir el RenderingStyle en QVariant i viceversa.
    QVariant toVariant() const;
    static RenderingStyle fromVariant(const QVariant &variant);

private:

    /// Mètode de rendering.
    Method m_method;
    /// Il·luminació o no.
    bool m_shading;
    /// Paràmetres d'il·luminació.
    double m_ambientCoefficient;
    double m_diffuseCoefficient;
    double m_specularCoefficient;
    double m_specularPower;
    /// Funció de transferència.
    TransferFunction m_transferFunction;
    /// Iso-valor per les iso-superfícies.
    double m_isoValue;

};

}

#endif
