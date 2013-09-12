#ifndef UDGVOLUMEDISPLAYUNITHANDLERFACTORY_H
#define UDGVOLUMEDISPLAYUNITHANDLERFACTORY_H

#include <QList>

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
    GenericVolumeDisplayUnitHandler* createVolumeDisplayUnitHandler(Volume *input);
    GenericVolumeDisplayUnitHandler* createVolumeDisplayUnitHandler(QList<Volume*> inputs);

private:
    /// Concrete creators for the different kind of handlers
    SingleVolumeDisplayUnitHandler* createSingleVolumeDisplayUnitHandler(Volume *input);
    GenericVolumeDisplayUnitHandler* createGenericVolumeDisplayUnitHandler(QList<Volume*> inputs);
    PairedVolumeDisplayUnitHandler* createPairedVolumeDisplayUnitHandler(QList<Volume*> inputs);
    
    /// Chooses the best handler among the family of paired handlers
    PairedVolumeDisplayUnitHandler* chooseBestPairedVolumeDisplayUnitHandler(QList<Volume*> inputs);
};

} // End namespace udg

#endif
