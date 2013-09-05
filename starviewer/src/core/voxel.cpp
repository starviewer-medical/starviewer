#include "voxel.h"

#include <QString>
#include <QObject>
#include <limits>

namespace udg {

Voxel::Voxel()
{
}

Voxel::~Voxel()
{
}

void Voxel::addComponent(double x)
{
    m_values << x;
}

double Voxel::getComponent(int i) const
{
    if (i > m_values.size() || i < 0)
    {
        return std::numeric_limits<double>::quiet_NaN();
    }
    else
    {
        return m_values.at(i);
    }
}

int Voxel::getNumberOfComponents() const
{
    return m_values.size();
}

bool Voxel::isEmpty() const
{
    return m_values.isEmpty();
}

void Voxel::reset()
{
    m_values.clear();
}

QString Voxel::getAsQString()
{
    QString valueString;
    
    switch (m_values.size())
    {
        case 0:
            valueString = QObject::tr("N/A");
            break;

        case 1:
            valueString = QString("%1").arg(m_values.at(0));
            break;

        default:
            valueString = QString("(%1").arg(m_values.at(0));
            for (int i = 1; i < m_values.size(); ++i)
            {
                valueString += QString(", %1").arg(m_values.at(i));
            }
            valueString += ")";
            break;
    }

    return valueString;
}

} // End namespace udg
