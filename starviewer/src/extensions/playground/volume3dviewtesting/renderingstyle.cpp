#include "renderingstyle.h"

#include <QVariant>


namespace udg {


QVariant RenderingStyle::toVariant() const
{
    QMap<QString, QVariant> map;
    map["diffuseLighting"] = diffuseLighting;
    map["specularLighting"] = specularLighting;
    map["specularPower"] = specularPower;
    map["transferFunction"] = transferFunction.toVariant();

    return map;
}


RenderingStyle RenderingStyle::fromVariant( const QVariant &variant )
{
    RenderingStyle renderingStyle;
    QMap<QString, QVariant> map = variant.toMap();

    renderingStyle.diffuseLighting = map["diffuseLighting"].toBool();
    renderingStyle.specularLighting = map["specularLighting"].toBool();
    renderingStyle.specularPower = map["specularPower"].toDouble();
    renderingStyle.transferFunction = TransferFunction::fromVariant( map["transferFunction"] );

    return renderingStyle;
}


}
