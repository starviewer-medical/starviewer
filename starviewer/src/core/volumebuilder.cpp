#ifndef UDGVOLUMEBUILDER_CPP
#define UDGVOLUMEBUILDER_CPP

#include "volumebuilder.h"

namespace udg {

VolumeBuilder::VolumeBuilder()
{
}

VolumeBuilder::~VolumeBuilder()
{
}

void VolumeBuilder::setSeriesDescription(QString description)
{
    m_seriesDescription = description;
}

QString VolumeBuilder::getSeriesDescription() const
{
    return m_seriesDescription;
}
}

#endif
