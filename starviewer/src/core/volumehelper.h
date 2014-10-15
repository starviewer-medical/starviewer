#ifndef UDGVOLUMEHELPER_H
#define UDGVOLUMEHELPER_H

namespace udg {

class Volume;

/**
    Helper class to get data from Volume
 */
class VolumeHelper {
public:
    VolumeHelper();
    ~VolumeHelper();

    /// Returns true if the volume is a primary PET acquisition
    static bool isPrimaryPET(Volume *volume);

    /// Returns true if the volume is a primary NM acquisition
    static bool isPrimaryNM(Volume *volume);
};

} // End namespace udg

#endif
