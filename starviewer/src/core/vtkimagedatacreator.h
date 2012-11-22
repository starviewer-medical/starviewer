#ifndef VTKIMAGEDATACREATOR_H
#define VTKIMAGEDATACREATOR_H

#include <vtkSmartPointer.h>

class vtkImageData;

namespace udg {

/**
 * Classe que permet crear un objecte vtkImageData amb les propietats desitjades. De moment sempre amb 1 component escalar només.
 */
class VtkImageDataCreator {

public:

    VtkImageDataCreator();

    /// Assigna l'origen del vtkImageData que es crearà.
    void setOrigin(double origin[3]);
    /// Assigna l'spacing del vtkImageData que es crearà.
    void setSpacing(double spacing[3]);
    /// Crea i retorna l'objecte vtkImageData amb les dimensions donades. Funciona per tots els tipus de dades numèrics.
    template <class T> vtkSmartPointer<vtkImageData> createVtkImageData(int width, int height, int depth, const T *data);

private:

    /// Crida el mètode SetScalarTypeTo*() d'imageData amb el tipus adequat.
    template <class T> static void setImageDataScalarType(vtkImageData *imageData);

private:

    double m_origin[3];
    double m_spacing[3];

};

} // namespace udg

#endif // VTKIMAGEDATACREATOR_H
