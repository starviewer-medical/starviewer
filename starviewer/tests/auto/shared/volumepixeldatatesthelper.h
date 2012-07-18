#ifndef VOLUMEPIXELDATATESTHELPER_H
#define VOLUMEPIXELDATATESTHELPER_H

#include "volumepixeldata.h"

using namespace udg;

namespace testing {

/// Classe de suport al testing de VolumePixelData.
class VolumePixelDataTestHelper {

public:

    /// Crea un VolumePixelData amb els paràmetres donats, l'omple de dades i el retorna.
    static VolumePixelData* createVolumePixelData(int dimensions[3], int extent[6], double spacing[3], double origin[3]);

    /// Crea dos objectes, un itk::Image i un vtkImageData, i els emplena amb els paràmetres donats perquè siguin iguals. També n'omple les dades.
    static void createItkAndVtkImages(int dimensions[3], int startIndex[3], double spacing[3], double origin[3], VolumePixelData::ItkImageTypePointer &itkImage,
                                      vtkSmartPointer<vtkImageData> &vtkImage);

};

}

#endif // VOLUMEPIXELDATATESTHELPER_H
