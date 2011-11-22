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

bool RenderingStyle::getDiffuseLighting() const
{
    return m_diffuseLighting;
}

void RenderingStyle::setDiffuseLighting(bool diffuseLighting)
{
    m_diffuseLighting = diffuseLighting;
}

bool RenderingStyle::getSpecularLighting() const
{
    return m_specularLighting;
}

void RenderingStyle::setSpecularLighting(bool specularLighting)
{
    m_specularLighting = specularLighting;
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
    map["diffuseLighting"] = m_diffuseLighting;
    map["specularLighting"] = m_specularLighting;
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
    renderingStyle.m_diffuseLighting = map["diffuseLighting"].toBool();
    renderingStyle.m_specularLighting = map["specularLighting"].toBool();
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
