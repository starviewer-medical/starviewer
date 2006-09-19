/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#ifndef UDGVOLUME_H
#define UDGVOLUME_H

#include <itkImage.h>
// Els filtres per passar itk<=>vtk: InsightApplications/auxiliary/vtk --> ho tenim a /tools
#include "itkImageToVTKImageFilter.h" //Converts an ITK image into a VTK image and plugs a itk data pipeline to a VTK datapipeline.   
#include "itkVTKImageToImageFilter.h" // Converts a VTK image into an ITK image and plugs a vtk data pipeline to an ITK datapipeline.   
#include <QString>

// FWD declarations
class vtkImageData;

namespace udg {

class VolumeSourceInformation;
/**
    Aquesta classe respresenta un volum de dades. Aquesta serà la classe on es guardaran les dades que voldrem tractar. Ens donarà mètodes
    per poder interrogar-la i obtenir les dades en el format que volguem: ITK, VTK, etc.
    
    @author Grup de Gràfics de Girona  ( GGG )
*/

class Volume
{

public:

    // Tipus d'imatge intern per defecte d'itk
    typedef signed int ItkPixelType;
    static const unsigned int VDimension = 3;
    
    typedef itk::Image<ItkPixelType, VDimension >    ItkImageType;
    typedef ItkImageType::Pointer  ItkImageTypePointer;
    typedef ItkImageType::RegionType   RegionType;
    typedef ItkImageType::SizeType     SizeType;
    typedef ItkImageType::IndexType    IndexType;
    
    /// tipus de punter de dades vtk
    typedef vtkImageData* VtkImageTypePointer;

    /// Filtres per importar/exportar
    typedef itk::ImageToVTKImageFilter< ItkImageType > ItkToVtkFilterType;
    typedef itk::VTKImageToImageFilter< ItkImageType > VtkToItkFilterType;
    
    Volume();
    Volume( ItkImageTypePointer itkImage );
    Volume( VtkImageTypePointer vtkImage );
    ~Volume();

    /**
       Li donem les dades en format ITK
       \sa getVtkData(), getItkData()
    */
    void setData( ItkImageTypePointer itkImage );
 
    /**
       Li donem les dades en format VTK
       \sa getVtkData(), getItkData()
    */
    void setData( VtkImageTypePointer vtkImage );
    
    /**
        Ens retorna la dades en format VTK
        \sa setData()
    */
    VtkImageTypePointer getVtkData();

    /**
        Ens retorna les dades en format ITK
        \sa setData()
    */
    ItkImageTypePointer getItkData();

    /// Actualitza la informació del volum
    void updateInformation();
    
    /// Obté l'origen del volum
    void getOrigin( double  xyz[3] );
    double *getOrigin();

    /// Obté l'espaiat del model al llarg dels eixos
    void getSpacing( double xyz[3] );
    double *getSpacing();

    /// Retorna la ¿bounding box?
    void getWholeExtent( int extent[6] );
    int *getWholeExtent();
    
    /// Retorna les dimensions del volum
    int *getDimensions();
    void getDimensions( int dims[3] );
    
    /// Assigna/Retorna la classe que conté tota la informació del volum
    void setVolumeSourceInformation( VolumeSourceInformation* information )
    {
        m_volumeInformation = information;
    }
    VolumeSourceInformation* getVolumeSourceInformation()
    {
        return m_volumeInformation;
    }
    
private:
    
    /// Mètode d'inicialització d'objectes comuns per als constructors
    void init();
    
    ItkImageTypePointer m_imageDataITK;
    VtkImageTypePointer m_imageDataVTK;
    
    ItkToVtkFilterType::Pointer m_itkToVtkFilter;
    VtkToItkFilterType::Pointer m_vtkToItkFilter;

    /// Dades relacionades amb el pacient i el volum
    VolumeSourceInformation* m_volumeInformation;
};

};  // end namespace udg

#endif
