#ifndef UDGIMAGEORIENTATIONSIGNALTOSYNCACTIONMAPPER_H
#define UDGIMAGEORIENTATIONSIGNALTOSYNCACTIONMAPPER_H

#include "signaltosyncactionmapper.h"

/// Includes needed to do the factory register
#include "imageorientationsyncaction.h"
#include "signaltosyncactionmapperfactoryregister.h"

namespace udg {

class PatientOrientation;

/**
    Implementation of SignalToSyncActionMapper to map patient orientation changes to ImageOrientationSyncAction
 */
class ImageOrientationSignalToSyncActionMapper : public SignalToSyncActionMapper {
Q_OBJECT
public:
    ImageOrientationSignalToSyncActionMapper(QObject *parent = 0);
    ~ImageOrientationSignalToSyncActionMapper();

    /// Maps current displayed image patient orientation
    virtual void mapProperty();

protected:
    void mapSignal();
    void unmapSignal();

protected slots:
    /// Maps given PatientOrientation to a PatientOrientationSyncAction with the corresponding values
    /// Should be connected to the corresponding signals on Q*Viewer
    void mapToSyncAction(const PatientOrientation &orientation);
};

/// With this line we register this mapper on the factory
static SignalToSyncActionMapperFactoryRegister<ImageOrientationSignalToSyncActionMapper> registerImageOrientationSignalToSyncActionMapper(ImageOrientationSyncAction().getMetaData());

} // End namespace udg

#endif
