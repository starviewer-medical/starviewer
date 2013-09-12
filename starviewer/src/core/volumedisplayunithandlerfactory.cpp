#include "volumedisplayunithandlerfactory.h"

#include "genericvolumedisplayunithandler.h"
#include "singlevolumedisplayunithandler.h"
#include "pairedvolumedisplayunithandler.h"
#include "petctvolumedisplayunithandler.h"

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

GenericVolumeDisplayUnitHandler* VolumeDisplayUnitHandlerFactory::createVolumeDisplayUnitHandler(Volume *input)
{
    return createSingleVolumeDisplayUnitHandler(input);
}

GenericVolumeDisplayUnitHandler* VolumeDisplayUnitHandlerFactory::createVolumeDisplayUnitHandler(QList<Volume*> inputs)
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

SingleVolumeDisplayUnitHandler* VolumeDisplayUnitHandlerFactory::createSingleVolumeDisplayUnitHandler(Volume *input)
{
    SingleVolumeDisplayUnitHandler *inputHandler = new SingleVolumeDisplayUnitHandler;
    inputHandler->setInput(input);

    return inputHandler;
}

PairedVolumeDisplayUnitHandler* VolumeDisplayUnitHandlerFactory::createPairedVolumeDisplayUnitHandler(QList<Volume*> inputs)
{
    PairedVolumeDisplayUnitHandler *inputHandler = chooseBestPairedVolumeDisplayUnitHandler(inputs);
    inputHandler->setInputs(inputs);
    
    return inputHandler;
}

GenericVolumeDisplayUnitHandler* VolumeDisplayUnitHandlerFactory::createGenericVolumeDisplayUnitHandler(QList<Volume*> inputs)
{
    GenericVolumeDisplayUnitHandler *inputHandler = new GenericVolumeDisplayUnitHandler;
    inputHandler->setInputs(inputs);

    return inputHandler;
}

PairedVolumeDisplayUnitHandler* VolumeDisplayUnitHandlerFactory::chooseBestPairedVolumeDisplayUnitHandler(QList<Volume*> inputs)
{
    QStringList modalities;

    foreach (Volume *volume, inputs)
    {
        modalities << volume->getImage(0)->getParentSeries()->getModality();
    }

    if (modalities.contains("CT") && modalities.contains("PT"))
    {
        return new PETCTVolumeDisplayUnitHandler;
    }
    else
    {
        return new PairedVolumeDisplayUnitHandler;
    }
}

} // End namespace udg
