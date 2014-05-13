/*************************************************************************************
  Copyright (C) 2014 Laboratori de Gràfics i Imatge, Universitat de Girona &
  Institut de Diagnòstic per la Imatge.
  Girona 2014. All rights reserved.
  http://starviewer.udg.edu

  This file is part of the Starviewer (Medical Imaging Software) open source project.
  It is subject to the license terms in the LICENSE file found in the top-level
  directory of this distribution and at http://starviewer.udg.edu/license. No part of
  the Starviewer (Medical Imaging Software) open source project, including this file,
  may be copied, modified, propagated, or distributed except according to the
  terms contained in the LICENSE file.
 *************************************************************************************/

#ifndef UDGVOLUMEPIXELDATA_H
#define UDGVOLUMEPIXELDATA_H

#include <QObject>
#include <QVector>

#include <itkImage.h>
#include <vtkSmartPointer.h>
// Els filtres per passar itk<=>vtk: InsightApplications/auxiliary/vtk --> ho tenim a /tools

// Converts an ITK image into a VTK image and plugs a itk data pipeline to a VTK datapipeline.
#include "itkImageToVTKImageFilter.h"
// Converts a VTK image into an ITK image and plugs a vtk data pipeline to an ITK datapipeline.
#include "itkVTKImageToImageFilter.h"

class vtkImageData;

namespace udg {

class VolumePixelDataIterator;
class Voxel;

/**
    Classe que té com a responsabilitat mantenir el pixel data d'un Volume.
    El pixel data d'un volume és el lloc de memòria on es guarden els diferents valors de voxel d'un Volume.
  */
class VolumePixelData : public QObject {
Q_OBJECT
public:

    /// Tipus d'imatge intern per defecte d'itk
    typedef signed short int ItkPixelType;
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

    /// Creem les dades a partir d'un buffer d'unsigned chars
    /// L'extent ha de ser coherent amb la mida de les dades del buffer i els bytesPerPixel indicats
    /// Si deleteData = cert, aquesta classe s'encarregarà de destruir el buffer quan es destrueixi aquest objecte
    /// Si deleteData = fals, (per defecte) no esborrarà el buffer
    /// Les característiques d'spacing i origin no s'assignaran amb aquest mètode. Això caldrà fer-ho accedint posteriorment a les dades vtk
    void setData(unsigned char *data, int extent[6], int bytesPerPixel, bool deleteData = false);

    /// Sets the number of phases of this pixel data.
    /// This information is needed to be able to access to the right pixels when accessing through world coordinate
    /// The minimum value must be 1, is less than, the method will do nothing
    void setNumberOfPhases(int numberOfPhases);
    
    /// Retorna cert si conté dades carregades.
    bool isLoaded() const;

    /// Returns a pointer to the raw pixel data at index [x, y, z]. Avoid its use if possible and prefer using an iterator instead.
    void* getScalarPointer(int x, int y, int z);
    /// Returns a pointer to the raw pixel data. Avoid its use if possible and prefer using an iterator instead.
    void* getScalarPointer();

    /// Returns a VolumePixelDataIterator pointing to the voxel at index [x, y, z].
    VolumePixelDataIterator getIterator(int x, int y, int z);
    /// Returns a VolumePixelDataIterator pointing to the first voxel.
    VolumePixelDataIterator getIterator();

    /// Donada una coordenada de món, ens dóna l'índex del vòxel corresponent.
    /// Si la coordenada està dins del volum retorna true, false altrament.
    /// TODO S'espera que la coordenada sigui dins del món VTK!
    /// Caldria determinar si ha de ser així o hauria de ser DICOM o en un altre sistema.
    /// HACK El paràmetre phaseNumber és necessari per poder calcular l'índex correcte dins del volum corresponent a la fase actual
    bool computeCoordinateIndex(const double coordinate[3], int index[3], int phaseNumber = 0);

    /// Donada una coordenada de món, ens dóna el valor del vòxel corresponent.
    /// TODO S'espera que la coordenada sigui dins del món VTK!
    /// Caldria determinar si ha de ser així o hauria de ser DICOM o en un altre sistema.
    /// HACK El paràmetre phaseNumber és necessari per poder calcular l'índex correcte dins del volum corresponent a la fase actual
    Voxel getVoxelValue(double coordinate[3], int phaseNumber = 0);

    /// Returns the voxel corresponding to the given index. If index is out of range, a default constructed value will be returned.
    Voxel getVoxelValue(int index[3]);

    /// S'encarrega de convertir el VolumePixelData en un pixel data neutre que permet que es faci servir en casos en
    /// els que ens quedem sense memòria o ens trobem amb altres problemes a l'hora d'intentar allotjar-ne un en memòria
    void convertToNeutralPixelData();

    /// Assigna/Obté l'origen
    void setOrigin(double origin[3]);
    void setOrigin(double x, double y, double z);
    void getOrigin(double origin[3]);

    /// Assigna/Obté l'espaiat del model al llarg dels eixos
    void setSpacing(double spacing[3]);
    void setSpacing(double x, double y, double z);
    void getSpacing(double spacing[3]);

    /// Retorna l'extent
    void getExtent(int extent[6]);

    /// Retorna el nombre de components escalars
    int getNumberOfScalarComponents();

    /// Retorna el nombre de components escalars
    int getScalarSize();
    
    /// Obté el tipus d'escalars
    int getScalarType();

    //  Obté el nombre de punts
    int getNumberOfPoints();
   
private:
    /// Filtres per importar/exportar
    typedef itk::ImageToVTKImageFilter<ItkImageType> ItkToVtkFilterType;
    typedef itk::VTKImageToImageFilter<ItkImageType> VtkToItkFilterType;

    /// Les dades en format vtk
    vtkSmartPointer<vtkImageData> m_imageDataVTK;

    /// Indica si conté dades carregades o no.
    bool m_loaded;

    /// Number of phases of the pixel data. Its minimum value must be 1
    int m_numberOfPhases;
    
    /// Filtres per passar de vtk a itk
    ItkToVtkFilterType::Pointer m_itkToVtkFilter;
    VtkToItkFilterType::Pointer m_vtkToItkFilter;
};

}

#endif // UDGVOLUMEPIXELDATA_H
