#ifndef UDGVOLUMEPIXELDATA_H
#define UDGVOLUMEPIXELDATA_H

#include <QObject>

#include <itkImage.h>
#include <vtkSmartPointer.h>
// Els filtres per passar itk<=>vtk: InsightApplications/auxiliary/vtk --> ho tenim a /tools
#include "itkImageToVTKImageFilter.h" //Converts an ITK image into a VTK image and plugs a itk data pipeline to a VTK datapipeline.
#include "itkVTKImageToImageFilter.h" // Converts a VTK image into an ITK image and plugs a vtk data pipeline to an ITK datapipeline.

class vtkImageData;

namespace udg {

/**
  Classe que té com a responsabilitat mantenir el pixel data d'un Volume.
  El pixel data d'un volume és el lloc de memòria on es guarden els diferents valors de voxel d'un Volume.
  */

class VolumePixelData : public QObject {
Q_OBJECT
public:
    /// Tipus de vòxel del volum
    typedef signed short int VoxelType;
    /// Tipus d'imatge intern per defecte d'itk
    typedef VoxelType ItkPixelType;
    static const unsigned int VDimension = 3;

    typedef itk::Image<ItkPixelType, VDimension> ItkImageType;
    typedef ItkImageType::Pointer ItkImageTypePointer;

    explicit VolumePixelData(QObject *parent = 0);

    /// Assignem/Retornem les dades en format ITK
    void setData(ItkImageTypePointer itkImage);
    ItkImageTypePointer getItkData();

    /// Assignem/Retornem les dades en format VTK
    void setData(vtkImageData *vtkImage);
    vtkImageData* getVtkData();

    /// Obtenim el punter a les dades que es troben en l'índex donat. És un accés a molt baix nivell, ja que obtenim
    /// el punter de les dades. Retornem el punter transformat al tipus natiu de dades VoxelType.
    VoxelType* getScalarPointer(int x, int y, int z);

    /// Donada una coordenada de món, ens dóna el valor del vòxel corresponent.
    /// Si la coordenada està dins del volum retorna true, false altrament.
    /// TODO S'espera que la coordenada sigui dins del món VTK!
    /// Caldria determinar si ha de ser així o hauria de ser DICOM o en un altre sistema.
    bool getVoxelValue(double coordinate[3], VoxelType &voxelValue);

    /// S'encarrega de convertir el VolumePixelData en un pixel data neutre que permet que es faci servir en casos en
    /// els que ens quedem sense memòria o ens trobem amb altres problemes a l'hora d'intentar allotjar-ne un en memòria
    void convertToNeutralPixelData();

private:
    /// Filtres per importar/exportar
    typedef itk::ImageToVTKImageFilter<ItkImageType> ItkToVtkFilterType;
    typedef itk::VTKImageToImageFilter<ItkImageType> VtkToItkFilterType;

    /// Les dades en format vtk
    vtkSmartPointer<vtkImageData> m_imageDataVTK;

    /// Filtres per passar de vtk a itk
    ItkToVtkFilterType::Pointer m_itkToVtkFilter;
    VtkToItkFilterType::Pointer m_vtkToItkFilter;
};

}

#endif // UDGVOLUMEPIXELDATA_H
