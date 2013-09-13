#ifndef UDGVOLUMEDISPLAYUNITHANDLERFACTORY_H
#define UDGVOLUMEDISPLAYUNITHANDLERFACTORY_H

#include <QList>
#include <QSharedPointer>

namespace udg {

class Volume;
class GenericVolumeDisplayUnitHandler;
class SingleVolumeDisplayUnitHandler;
class PairedVolumeDisplayUnitHandler;

/**
    Class to create the *VolumeDisplayUnitHandler that best fits for the given input(s)
 */
class VolumeDisplayUnitHandlerFactory {
public:
    VolumeDisplayUnitHandlerFactory();
    ~VolumeDisplayUnitHandlerFactory();

    /// Returns the proper display unit handler for the given input(s)
    QSharedPointer<GenericVolumeDisplayUnitHandler> createVolumeDisplayUnitHandler(Volume *input);
    QSharedPointer<GenericVolumeDisplayUnitHandler> createVolumeDisplayUnitHandler(QList<Volume*> inputs);

private:
    /// Concrete creators for the different kind of handlers
    QSharedPointer<SingleVolumeDisplayUnitHandler> createSingleVolumeDisplayUnitHandler(Volume *input);
    QSharedPointer<GenericVolumeDisplayUnitHandler> createGenericVolumeDisplayUnitHandler(QList<Volume*> inputs);
    QSharedPointer<PairedVolumeDisplayUnitHandler> createPairedVolumeDisplayUnitHandler(QList<Volume*> inputs);
    
    /// Chooses the best handler among the family of paired handlers
    QSharedPointer<PairedVolumeDisplayUnitHandler> chooseBestPairedVolumeDisplayUnitHandler(QList<Volume*> inputs);
};

} // End namespace udg

#endif
