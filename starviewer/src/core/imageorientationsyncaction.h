#ifndef UDGIMAGEORIENTATIONSYNCACTION_H
#define UDGIMAGEORIENTATIONSYNCACTION_H

#include "syncaction.h"
#include "patientorientation.h"

namespace udg {

/**
    Implementation of a SyncAction for a patient orientation
    The set patient orientation on setImageOrientation() will be applied when run() is called
 */
class ImageOrientationSyncAction : public SyncAction {
public:
    ImageOrientationSyncAction();
    ~ImageOrientationSyncAction();

    /// Sets the patient orientation to be synched
    void setImageOrientation(const PatientOrientation &orientation);
    
    void run(QViewer *viewer);

protected:
    void setupMetaData();
    void setupDefaultSyncCriteria();

protected:
    /// PatientOrientation that will be applied on run()
    PatientOrientation m_orientation;
};

} // End namespace udg

#endif
