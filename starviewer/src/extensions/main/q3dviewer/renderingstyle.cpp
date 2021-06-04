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

#include "renderingstyle.h"

#include <QVariant>

namespace udg {

RenderingStyle::Method RenderingStyle::getMethod() const
{
    return m_method;
}

void RenderingStyle::setMethod(Method method)
{
    m_method = method;
}

bool RenderingStyle::getShading() const
{
    return m_shading;
}

void RenderingStyle::setShading(bool shading)
{
    m_shading = shading;
}

double RenderingStyle::getAmbientCoefficient() const
{
    return m_ambientCoefficient;
}

void RenderingStyle::setAmbientCoefficient(double ambientCoefficient)
{
    m_ambientCoefficient = ambientCoefficient;
}

double RenderingStyle::getDiffuseCoefficient() const
{
    return m_diffuseCoefficient;
}

void RenderingStyle::setDiffuseCoefficient(double diffuseCoefficient)
{
    m_diffuseCoefficient = diffuseCoefficient;
}

double RenderingStyle::getSpecularCoefficient() const
{
    return m_specularCoefficient;
}

void RenderingStyle::setSpecularCoefficient(double specularCoefficient)
{
    m_specularCoefficient = specularCoefficient;
}

double RenderingStyle::getSpecularPower() const
{
    return m_specularPower;
}

void RenderingStyle::setSpecularPower(double specularPower)
{
    m_specularPower = specularPower;
}

const TransferFunction& RenderingStyle::getTransferFunction() const
{
    return m_transferFunction;
}

void RenderingStyle::setTransferFunction(const TransferFunction &transferFunction)
{
    m_transferFunction = transferFunction;
}

double RenderingStyle::getIsoValue() const
{
    return m_isoValue;
}

void RenderingStyle::setIsoValue(double isoValue)
{
    m_isoValue = isoValue;
}

QVariant RenderingStyle::toVariant() const
{
    QMap<QString, QVariant> map;
    map["method"] = m_method;
    map["shading"] = m_shading;
    map["ambientCoefficient"] = m_ambientCoefficient;
    map["diffuseCoefficient"] = m_diffuseCoefficient;
    map["specularCoefficient"] = m_specularCoefficient;
    map["specularPower"] = m_specularPower;
    map["transferFunction"] = m_transferFunction.toVariant();
    map["isoValue"] = m_isoValue;

    return map;
}

RenderingStyle RenderingStyle::fromVariant(const QVariant &variant)
{
    RenderingStyle renderingStyle;
    QMap<QString, QVariant> map = variant.toMap();

    renderingStyle.m_method = static_cast<Method>(map["method"].toInt());
    renderingStyle.m_shading = map["shading"].toBool();
    renderingStyle.m_ambientCoefficient = map["ambientCoefficient"].toDouble();
    renderingStyle.m_diffuseCoefficient = map["diffuseCoefficient"].toDouble();
    renderingStyle.m_specularCoefficient = map["specularCoefficient"].toDouble();
    renderingStyle.m_specularPower = map["specularPower"].toDouble();
    renderingStyle.m_transferFunction = TransferFunction::fromVariant(map["transferFunction"]);
    renderingStyle.m_isoValue = map["isoValue"].toDouble();

    return renderingStyle;
}

}
