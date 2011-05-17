#include "colortransferfunction.h"

#include <QColor>
#include <QVariant>

#include <vtkColorTransferFunction.h>

namespace udg {

template <>
QColor TransferFunctionTemplate<QColor>::linearInterpolation(const QColor &a, const QColor &b, double alpha)
{
    Q_ASSERT(a.isValid());
    Q_ASSERT(b.isValid());
    Q_ASSERT(!MathTools::isNaN(alpha));
    double red = a.redF() + alpha * (b.redF() - a.redF());
    double green = a.greenF() + alpha * (b.greenF() - a.greenF());
    double blue = a.blueF() + alpha * (b.blueF() - a.blueF());
    return QColor::fromRgbF(red, green, blue);
}

ColorTransferFunction::ColorTransferFunction()
    : m_vtkColorTransferFunction(0)
{
}

ColorTransferFunction::ColorTransferFunction(const ColorTransferFunction &colorTransferFunction)
    : TransferFunctionTemplate<QColor>(colorTransferFunction), m_vtkColorTransferFunction(0)
{
}

ColorTransferFunction::~ColorTransferFunction()
{
    if (m_vtkColorTransferFunction)
    {
        m_vtkColorTransferFunction->Delete();
    }
}

ColorTransferFunction& ColorTransferFunction::operator =(const ColorTransferFunction &colorTransferFunction)
{
    if (this != &colorTransferFunction)
    {
        TransferFunctionTemplate<QColor>::operator =(colorTransferFunction);
        m_vtkColorTransferFunction = 0;
    }

    return *this;
}

void ColorTransferFunction::set(double x, const QColor &color)
{
    Q_ASSERT(color.isValid());
    TransferFunctionTemplate<QColor>::set(x, color);
}

void ColorTransferFunction::set(double x, int red, int green, int blue)
{
    Q_ASSERT(red >= 0 && red <= 255);
    Q_ASSERT(green >= 0 && green <= 255);
    Q_ASSERT(blue >= 0 && blue <= 255);
    set(x, QColor(red, green, blue));
}

void ColorTransferFunction::set(double x, double red, double green, double blue)
{
    Q_ASSERT(red >= 0.0 && red <= 1.0);
    Q_ASSERT(green >= 0.0 && green <= 1.0);
    Q_ASSERT(blue >= 0.0 && blue <= 1.0);
    set(x, QColor::fromRgbF(red, green, blue));
}

vtkColorTransferFunction* ColorTransferFunction::vtkColorTransferFunction() const
{
    if (m_vtkColorTransferFunction)
    {
        m_vtkColorTransferFunction->Delete();
    }
    m_vtkColorTransferFunction = vtkColorTransferFunction::New();

    QMapIterator<double, QColor> it(m_map);

    while (it.hasNext())
    {
        it.next();
        m_vtkColorTransferFunction->AddRGBPoint(it.key(), it.value().redF(), it.value().greenF(), it.value().blueF());
    }

    return m_vtkColorTransferFunction;
}

QString ColorTransferFunction::toString() const
{
    QString string;
    QMapIterator<double, QColor> it(m_map);

    while (it.hasNext())
    {
        it.next();
        string += QString("x = %1, color = (%2, %3, %4)\n").arg(it.key()).arg(it.value().redF()).arg(it.value().greenF()).arg(it.value().blueF());
    }

    return string;
}

QVariant ColorTransferFunction::toVariant() const
{
    QMap<QString, QVariant> map;
    QMapIterator<double, QColor> it(m_map);

    while (it.hasNext())
    {
        it.next();
        map[QString::number(it.key())] = it.value();
    }

    QMap<QString, QVariant> variant;
    variant["name"] = m_name;
    variant["map"] = map;

    return variant;
}

ColorTransferFunction ColorTransferFunction::fromVariant(const QVariant &variant)
{
    ColorTransferFunction colorTransferFunction;
    QMap<QString, QVariant> variantMap = variant.toMap();
    colorTransferFunction.m_name = variantMap.value("name").toString();
    QMap<QString, QVariant> map = variantMap.value("map").toMap();
    QMapIterator<QString, QVariant> it(map);

    while (it.hasNext())
    {
        it.next();
        colorTransferFunction.m_map[it.key().toDouble()] = it.value().value<QColor>();
    }

    return colorTransferFunction;
}

} // End namespace udg
