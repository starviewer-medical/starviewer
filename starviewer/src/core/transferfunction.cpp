#include "transferfunction.h"

#include <QColor>
#include <QVariant>

#include <vtkLookupTable.h>


namespace udg {


TransferFunction::TransferFunction()
    : m_changed(true)
{
}


TransferFunction::TransferFunction(vtkLookupTable *lookupTable)
    : m_changed(true)
{
    Q_ASSERT(lookupTable);

    // Transformem la vtkLookupTable al nostre format
    double range[2];
    lookupTable->GetTableRange(range);

    // Calculem quina és la diferència entre valor i valor de la taula
    double step = (range[1] - range[0]) / lookupTable->GetNumberOfTableValues();

    // Recorrem la vtkLookupTable i inserim els valors al nostre format
    for (double value = range[0]; value < range[1]; value += step)
    {
        double rgb[3];
        double opacity;
        lookupTable->GetColor(value, rgb);
        opacity = lookupTable->GetOpacity(value);
        m_color.set(value, rgb[0], rgb[1], rgb[2]);
        m_opacity.set(value, opacity);
    }
}


bool TransferFunction::operator ==(const TransferFunction &transferFunction) const
{
    return m_color == transferFunction.m_color && m_opacity == transferFunction.m_opacity;
}


const QString& TransferFunction::name() const
{
    return m_color.name();
}


void TransferFunction::setName(const QString &name)
{
    m_color.setName(name);
    m_opacity.setName(name);
}


QColor TransferFunction::get(double x) const
{
    QColor rgba = m_color.get(x);
    rgba.setAlphaF(m_opacity.get(x));
    return rgba;
}


QColor TransferFunction::getColor(double x) const
{
    return m_color.get(x);
}


double TransferFunction::getOpacity(double x) const
{
    return m_opacity.get(x);
}


void TransferFunction::set(double x, const QColor &color, double opacity)
{
    m_color.set(x, color);
    m_opacity.set(x, opacity);
    m_changed = true;
}


void TransferFunction::set(double x, int red, int green, int blue, double opacity)
{
    m_color.set(x, red, green, blue);
    m_opacity.set(x, opacity);
    m_changed = true;
}


void TransferFunction::set(double x, double red, double green, double blue, double opacity)
{
    m_color.set(x, red, green, blue);
    m_opacity.set(x, opacity);
    m_changed = true;
}


void TransferFunction::setColor(double x, const QColor &color)
{
    m_color.set(x, color);
    m_changed = true;
}


void TransferFunction::setColor(double x, int red, int green, int blue)
{
    m_color.set(x, red, green, blue);
    m_changed = true;
}


void TransferFunction::setColor(double x, double red, double green, double blue)
{
    m_color.set(x, red, green, blue);
    m_changed = true;
}


void TransferFunction::setOpacity(double x, double opacity)
{
    m_opacity.set(x, opacity);
    m_changed = true;
}


void TransferFunction::unset(double x)
{
    m_color.unset(x);
    m_opacity.unset(x);
    m_changed = true;
}


void TransferFunction::unsetColor(double x)
{
    m_color.unset(x);
    m_changed = true;
}


void TransferFunction::unsetOpacity(double x)
{
    m_opacity.unset(x);
    m_changed = true;
}


void TransferFunction::clear()
{
    m_color.clear();
    m_opacity.clear();
    m_changed = true;
}


void TransferFunction::clearColor()
{
    m_color.clear();
    m_changed = true;
}


void TransferFunction::clearOpacity()
{
    m_opacity.clear();
    m_changed = true;
}


QList<double>& TransferFunction::keys() const
{
    updateKeys();
    return m_keys;
}


QList<double> TransferFunction::keys(double begin, double end) const
{
    Q_ASSERT(!MathTools::isNaN(begin));
    Q_ASSERT(!MathTools::isNaN(end));
    Q_ASSERT(begin <= end);

    updateKeys();

    QList<double>::const_iterator lowerBound = qLowerBound(m_keys, begin);
    QList<double>::const_iterator itEnd = m_keys.constEnd();
    QList<double> keys;

    while (lowerBound != itEnd && *lowerBound <= end) keys << *(lowerBound++);

    return keys;
}


QList<double> TransferFunction::keysNear(double x, double distance) const
{
    Q_ASSERT(!MathTools::isNaN(x));
    Q_ASSERT(!MathTools::isNaN(distance));
    Q_ASSERT(distance >= 0.0);
    return keys(x - distance, x + distance);
}


QList<double> TransferFunction::colorKeys() const
{
    return m_color.keys();
}


QList<double> TransferFunction::opacityKeys() const
{
    return m_opacity.keys();
}


void TransferFunction::trim(double x1, double x2)
{
    m_color.trim(x1, x2);
    m_opacity.trim(x1, x2);
    m_changed = true;
}


TransferFunction TransferFunction::to01(double x1, double x2) const
{
    Q_ASSERT(!MathTools::isNaN(x1));
    Q_ASSERT(!MathTools::isNaN(x2));
    Q_ASSERT(x1 < x2);

    updateKeys();

    double shift = -x1, scale = 1.0 / (x2 - x1);
    TransferFunction transferFunction01;

    foreach (double x, m_keys) transferFunction01.set((x + shift) * scale, getColor(x), getOpacity(x));

    return transferFunction01;
}


TransferFunction TransferFunction::simplify() const
{
    TransferFunction simplified(*this);
    simplified.m_color.simplify();
    simplified.m_opacity.simplify();
    simplified.m_changed = true;
    return simplified;
}


TransferFunction TransferFunction::normalize() const
{
    updateKeys();

    TransferFunction normalized;
    QListIterator<double> it(m_keys);

    while (it.hasNext())
    {
        double x = it.next();
        normalized.set(x, getColor(x), getOpacity(x));
    }

    normalized.setName(name());

    return normalized;
}


const ColorTransferFunction& TransferFunction::colorTransferFunction() const
{
    return m_color;
}


void TransferFunction::setColorTransferFunction(const ColorTransferFunction &colorTransferFunction)
{
    m_color = colorTransferFunction;
    m_color.setName(m_opacity.name());
    m_changed = true;
}


const OpacityTransferFunction& TransferFunction::opacityTransferFunction() const
{
    return m_opacity;
}


void TransferFunction::setOpacityTransferFunction(const OpacityTransferFunction &opacityTransferFunction)
{
    m_opacity = opacityTransferFunction;
    m_opacity.setName(m_color.name());
    m_changed = true;
}


vtkColorTransferFunction* TransferFunction::vtkColorTransferFunction() const
{
    return m_color.vtkColorTransferFunction();
}


vtkPiecewiseFunction* TransferFunction::vtkOpacityTransferFunction() const
{
    return m_opacity.vtkOpacityTransferFunction();
}


QString TransferFunction::toString() const
{
    return "Color:\n" + m_color.toString() + "Opacity:\n" + m_opacity.toString();
}


QVariant TransferFunction::toVariant() const
{
    QMap<QString, QVariant> variant;
    variant["color"] = m_color.toVariant();
    variant["opacity"] = m_opacity.toVariant();
    return variant;
}


TransferFunction TransferFunction::fromVariant(const QVariant &variant)
{
    TransferFunction transferFunction;
    const QMap<QString, QVariant> variantMap = variant.toMap();
    transferFunction.m_color = ColorTransferFunction::fromVariant(variantMap.value("color"));
    transferFunction.m_opacity = OpacityTransferFunction::fromVariant(variantMap.value("opacity"));
    return transferFunction;
}


void TransferFunction::updateKeys() const
{
    if (!m_changed) return;

    m_keys = m_color.keys();
    m_keys << m_opacity.keys();
    qSort(m_keys);

    for (int i = 0; i < m_keys.size() - 1; i++)
    {
        if (m_keys.at(i) == m_keys.at(i+1)) m_keys.removeAt(i+1);
    }

    m_changed = false;
}


}
