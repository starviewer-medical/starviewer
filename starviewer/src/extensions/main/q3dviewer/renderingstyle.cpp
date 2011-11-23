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

bool RenderingStyle::getContour() const
{
    return m_contour;
}

void RenderingStyle::setContour(bool contour)
{
    m_contour = contour;
}

double RenderingStyle::getContourThreshold() const
{
    return m_contourThreshold;
}

void RenderingStyle::setContourThreshold(double contourThreshold)
{
    m_contourThreshold = contourThreshold;
}

bool RenderingStyle::getObscurance() const
{
    return m_obscurance;
}

void RenderingStyle::setObscurance(bool obscurance)
{
    m_obscurance = obscurance;
}

RenderingStyle::ObscuranceQuality RenderingStyle::getObscuranceQuality() const
{
    return m_obscuranceQuality;
}

void RenderingStyle::setObscuranceQuality(ObscuranceQuality obscuranceQuality)
{
    m_obscuranceQuality = obscuranceQuality;
}

double RenderingStyle::getObscuranceFactor() const
{
    return m_obscuranceFactor;
}

void RenderingStyle::setObscuranceFactor(double obscuranceFactor)
{
    m_obscuranceFactor = obscuranceFactor;
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
    map["contour"] = m_contour;
    map["contourThreshold"] = m_contourThreshold;
    map["obscurance"] = m_obscurance;
    map["obscuranceQuality"] = m_obscuranceQuality;
    map["obscuranceFactor"] = m_obscuranceFactor;
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
    renderingStyle.m_contour = map["contour"].toBool();
    renderingStyle.m_contourThreshold = map["contourThreshold"].toDouble();
    renderingStyle.m_obscurance = map["obscurance"].toBool();
    renderingStyle.m_obscuranceQuality = static_cast<ObscuranceQuality>(map["obscuranceQuality"].toInt());
    renderingStyle.m_obscuranceFactor = map["obscuranceFactor"].toDouble();
    renderingStyle.m_isoValue = map["isoValue"].toDouble();

    return renderingStyle;
}

}
