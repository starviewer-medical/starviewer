/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
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

void VolumeBuilder::setSeriesDescription( QString description)
{
    m_seriesDescription = description;
}

QString VolumeBuilder::getSeriesDescription() const
{
    return m_seriesDescription;
}
}

#endif
