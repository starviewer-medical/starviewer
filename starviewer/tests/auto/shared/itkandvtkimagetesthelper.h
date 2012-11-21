#ifndef ITKANDVTKIMAGETESTHELPER_H
#define ITKANDVTKIMAGETESTHELPER_H

#include "volumepixeldata.h"

using namespace udg;

namespace testing {

/// Classe de suport al testing amb itk::Image i vtkImageData.
class ItkAndVtkImageTestHelper {

public:

    /// Crea una itk::Image amb les característiques donades i n'omple les dades.
    static VolumePixelData::ItkImageTypePointer createItkImage(int dimensions[3], int startIndex[3], double spacing[3], double origin[3]);

    /// Crea dos objectes, un itk::Image i un vtkImageData, i els emplena amb els paràmetres donats perquè siguin iguals. També n'omple les dades.
    static void createItkAndVtkImages(int dimensions[3], int startIndex[3], double spacing[3], double origin[3], VolumePixelData::ItkImageTypePointer &itkImage,
                                      vtkSmartPointer<vtkImageData> &vtkImage);

    /// Compara dos objectes vtkImageData fent servir l'API de QTest. Posa equal a cert si són iguals i a fals si són diferents.
    /// Per la manera com funcionen QCOMPARE i QVERIFY aquest mètode no pot retornar un valor directament, per això el retorna per paràmetre de sortida.
    static void compareVtkImageData(vtkImageData *actualImageData, vtkImageData *expectedImageData, bool &equal);

};

}

#endif // ITKANDVTKIMAGETESTHELPER_H
