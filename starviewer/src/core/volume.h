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
#include <QList>

// FWD declarations
class vtkImageData;

namespace udg {

class VolumeSourceInformation;
class Image;
class Series;

/**
    Aquesta classe respresenta un volum de dades. Aquesta serà la classe on es guardaran les dades que voldrem tractar. Ens donarà mètodes per poder obtenir les dades en el format que volguem: ITK, VTK, etc.

    Es pot inicialitzar amb dades de tipus itk o vtk amb el mètode \c setData() .
    Per raons d'eficiència, com que el programa principalment el que farà serà visualitzar es retindran les dades en format natiu vtk. Només es convertiran a itk quan es demanin explícitament.

    @author Grup de Gràfics de Girona  ( GGG )
*/
class Volume{
public:
    /// Tipus d'imatge intern per defecte d'itk
    typedef signed int ItkPixelType;
    static const unsigned int VDimension = 3;

    typedef itk::Image<ItkPixelType, VDimension >    ItkImageType;
    typedef ItkImageType::Pointer  ItkImageTypePointer;

    /// tipus de punter de dades vtk
    typedef vtkImageData *VtkImageTypePointer;

    /// Aquests enums indiquem quin criteri escollim per ordenar les imatges
    enum ImageOrderType{ OrderImageByNumber, OrderImageBySliceLocation, OrderImageByTemporalDimension };

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

    /// TODO: Repassar: això no hauria d'anar així.
    /// Retorna un nou Volume on la Data és només del sub volume indicat
    Volume *getSubVolume( int index );
    /// Reordena les llesques: específic CardiacMPRExtension
    Volume *orderSlices();

    /// Assignar/Obtenir el criteri d'ordenació de les imatges
    void setImageOrderCriteria( unsigned int orderCriteria );
    unsigned int getImageOrderCriteria() const;

    /// Afegim una imatge al conjunt d'imatges que composen el volum
    void addImage( Image *image );

    /// Assignem directament el conjunt d'imatges que composen aquest volum
    void setImages( const QList<Image *> &imageList );

    /// Mètode ràpid per obtenir la series a la que pertany aquest volum
    Series *getSeries();

private:
    /// Mètode d'inicialització d'objectes comuns per als constructors
    void init();

    /// Mètodes de prova per tractar diferents models de càrrega de dades "lazy"
    /// Carrega fent servir el vtkImageAppend
    void loadWithAppends();
    /// Allotja l'espai a memòria primer, després va inserint les imatges una a una
    void loadWithPreAllocateAndInsert();

    /// carrega les llesques a partir dels objectes Image
    void loadSlices( int method );

    /// reserva l'espai per la imatge vtk segons l'input d'imatges que tenim
    void allocateImageData();

private:
    /// Filtres per importar/exportar
    typedef itk::ImageToVTKImageFilter< ItkImageType > ItkToVtkFilterType;
    typedef itk::VTKImageToImageFilter< ItkImageType > VtkToItkFilterType;

    /// Les dades en format vtk
    VtkImageTypePointer m_imageDataVTK;

    /// Filtres per passar de vtk a itk
    ItkToVtkFilterType::Pointer m_itkToVtkFilter;
    VtkToItkFilterType::Pointer m_vtkToItkFilter;

    /// Dades relacionades amb el pacient i el volum
    VolumeSourceInformation* m_volumeInformation;

    /// Ens diu si les dades han estat carregades ja en memòria o no.
    /// Aquest membre el farem servir per aplicar el lazy loading
    bool m_dataLoaded;

    /// Criteri d'ordenació de les imatges
    unsigned int m_imageOrderCriteria;

    /// Conjunt d'imatges que composen el volum
    QList<Image *> m_imageSet;
};

};  // end namespace udg

#endif
