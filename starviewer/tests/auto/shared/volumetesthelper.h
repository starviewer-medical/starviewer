#ifndef VOLUMETESTHELPER_H
#define VOLUMETESTHELPER_H

#include <QString>

namespace udg{
    class Volume;
}

namespace testing
{

/// Classe que retorna Volums per utilitzar per testing
class VolumeTestHelper
{
public:
    /// Torna un Volume amb el número d'imatges, fase i imatges per fase en cada sèrie indicat.
    static udg::Volume* createVolume(int numberOfImages = 0, int numberOfPhases = 1, int numberOfSlicesPerPhase = 1);

    /// Torna un Volume amb el num d'imatges, fases i imatges, a més d'assignar-li al vtkImageData l'origin, spacing, dimensions i l'extent passats per paràmetre
    static udg::Volume* createVolumeWithParameters(int numberOfImages, int numberOfPhases, int numberOfSlicesPerPhase, double origin[3], double spacing[3], int dimensions[3], int extent[6]);

    /// Torna un Volume multiframe (amb més d'una imatge i totes amb el mateix path) amb el número d'imatges, fase i imatges per fase en cada sèrie indicat.
    static udg::Volume* createMultiframeVolume(int numberOfImages = 2, int numberOfPhases = 1, int numberOfSlicesPerPhase = 1);

    /// Elimina les imatges del volum cridant el destructor de l'estructura Patient/Study/Series/Image que pertany a cada imatge
    static void cleanUp(udg::Volume *volume);
};

}

#endif // VOLUMETESTHELPER_H
