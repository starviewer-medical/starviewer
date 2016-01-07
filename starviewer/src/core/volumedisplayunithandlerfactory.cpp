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

#include "volumedisplayunithandlerfactory.h"

#include "genericvolumedisplayunithandler.h"
#include "singlevolumedisplayunithandler.h"
#include "pairedvolumedisplayunithandler.h"
#include "petctvolumedisplayunithandler.h"
#include "petvolumedisplayunithandler.h"
#include "nmvolumedisplayunithandler.h"
#include "nmctvolumedisplayunithandler.h"

#include "volume.h"
#include "volumehelper.h"
#include "image.h"
#include "series.h"

#include <QStringList>

namespace udg {

VolumeDisplayUnitHandlerFactory::VolumeDisplayUnitHandlerFactory()
{
}

VolumeDisplayUnitHandlerFactory::~VolumeDisplayUnitHandlerFactory()
{
}

QSharedPointer<GenericVolumeDisplayUnitHandler> VolumeDisplayUnitHandlerFactory::createVolumeDisplayUnitHandler(Volume *input)
{
    return createSingleVolumeDisplayUnitHandler(input);
}

QSharedPointer<GenericVolumeDisplayUnitHandler> VolumeDisplayUnitHandlerFactory::createVolumeDisplayUnitHandler(const QList<Volume*> &inputs)
{
    switch (inputs.size())
    {
        case 1:
            return createSingleVolumeDisplayUnitHandler(inputs.first());
            break;

        case 2:
            return createPairedVolumeDisplayUnitHandler(inputs);
            break;
        
        default:
            return createGenericVolumeDisplayUnitHandler(inputs);
            break;
    }
}

QSharedPointer<SingleVolumeDisplayUnitHandler> VolumeDisplayUnitHandlerFactory::createSingleVolumeDisplayUnitHandler(Volume *input)
{
    QSharedPointer<SingleVolumeDisplayUnitHandler> inputHandler(chooseBestSingleVolumeDisplayUnitHandler(input));
    inputHandler->setInput(input);

    return inputHandler;
}

QSharedPointer<PairedVolumeDisplayUnitHandler> VolumeDisplayUnitHandlerFactory::createPairedVolumeDisplayUnitHandler(const QList<Volume*> &inputs)
{
    QSharedPointer<PairedVolumeDisplayUnitHandler> inputHandler(chooseBestPairedVolumeDisplayUnitHandler(inputs));
    inputHandler->setInputs(inputs);
    
    return inputHandler;
}

QSharedPointer<GenericVolumeDisplayUnitHandler> VolumeDisplayUnitHandlerFactory::createGenericVolumeDisplayUnitHandler(const QList<Volume*> &inputs)
{
    QSharedPointer<GenericVolumeDisplayUnitHandler> inputHandler(new GenericVolumeDisplayUnitHandler());
    inputHandler->setInputs(inputs);

    return inputHandler;
}

QSharedPointer<SingleVolumeDisplayUnitHandler> VolumeDisplayUnitHandlerFactory::chooseBestSingleVolumeDisplayUnitHandler(Volume *input)
{
    if (VolumeHelper::isPrimaryPET(input))
    {
        return QSharedPointer<SingleVolumeDisplayUnitHandler>(new PETVolumeDisplayUnitHandler());
    }
    else if (VolumeHelper::isPrimaryNM(input))
    {
        return QSharedPointer<SingleVolumeDisplayUnitHandler>(new NMVolumeDisplayUnitHandler());
    }
    else
    {
        return QSharedPointer<SingleVolumeDisplayUnitHandler>(new SingleVolumeDisplayUnitHandler());
    }
}

QSharedPointer<PairedVolumeDisplayUnitHandler> VolumeDisplayUnitHandlerFactory::chooseBestPairedVolumeDisplayUnitHandler(const QList<Volume*> &inputs)
{
    QStringList modalities;

    foreach (Volume *volume, inputs)
    {
        modalities << volume->getModality();
    }

    if (modalities.contains("CT") && modalities.contains("PT"))
    {
        return QSharedPointer<PairedVolumeDisplayUnitHandler>(new PETCTVolumeDisplayUnitHandler());
    }
    else if (modalities.contains("CT") && modalities.contains("NM"))
    {
        return QSharedPointer<PairedVolumeDisplayUnitHandler>(new NMCTVolumeDisplayUnitHandler());
    }
    else
    {
        return QSharedPointer<PairedVolumeDisplayUnitHandler>(new PairedVolumeDisplayUnitHandler());
    }
}

} // End namespace udg
