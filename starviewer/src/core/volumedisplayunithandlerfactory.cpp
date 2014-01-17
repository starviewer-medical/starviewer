#include "volumedisplayunithandlerfactory.h"

#include "genericvolumedisplayunithandler.h"
#include "singlevolumedisplayunithandler.h"
#include "pairedvolumedisplayunithandler.h"
#include "petctvolumedisplayunithandler.h"
#include "petvolumedisplayunithandler.h"

#include "volume.h"
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
    if (input->getModality() == "PT")
    {
        return QSharedPointer<SingleVolumeDisplayUnitHandler>(new PETVolumeDisplayUnitHandler());
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
    else
    {
        return QSharedPointer<PairedVolumeDisplayUnitHandler>(new PairedVolumeDisplayUnitHandler());
    }
}

} // End namespace udg
