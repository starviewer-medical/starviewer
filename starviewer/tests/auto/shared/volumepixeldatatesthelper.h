#ifndef VOLUMEPIXELDATATESTHELPER_H
#define VOLUMEPIXELDATATESTHELPER_H

namespace udg {

class VolumePixelData;

}

namespace testing {

/// Classe de suport al testing de VolumePixelData.
class VolumePixelDataTestHelper {

public:

    /// Crea un VolumePixelData amb els paràmetres donats, l'omple de dades i el retorna.
    static udg::VolumePixelData* createVolumePixelData(int dimensions[3], int extent[6], double spacing[3], double origin[3]);

};

}

#endif // VOLUMEPIXELDATATESTHELPER_H
