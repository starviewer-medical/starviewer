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
    QSharedPointer<GenericVolumeDisplayUnitHandler> createVolumeDisplayUnitHandler(const QList<Volume*> &inputs);

private:
    /// Concrete creators for the different kind of handlers
    QSharedPointer<SingleVolumeDisplayUnitHandler> createSingleVolumeDisplayUnitHandler(Volume *input);
    QSharedPointer<GenericVolumeDisplayUnitHandler> createGenericVolumeDisplayUnitHandler(const QList<Volume*> &inputs);
    QSharedPointer<PairedVolumeDisplayUnitHandler> createPairedVolumeDisplayUnitHandler(const QList<Volume*> &inputs);

    /// Chooses the best handler among the family of single handlers
    QSharedPointer<SingleVolumeDisplayUnitHandler> chooseBestSingleVolumeDisplayUnitHandler(Volume *input);

    /// Chooses the best handler among the family of paired handlers
    QSharedPointer<PairedVolumeDisplayUnitHandler> chooseBestPairedVolumeDisplayUnitHandler(const QList<Volume*> &inputs);
};

} // End namespace udg

#endif
