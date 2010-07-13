#ifndef UDGVOLUMEPIXELDATAREADERITKGDCM_H
#define UDGVOLUMEPIXELDATAREADERITKGDCM_H

#include "volumepixeldatareader.h"

#include "volume.h" // Pel tipus de dades itk definits

#include <itkImageSeriesReader.h>
#include <itkImageFileReader.h>
#include <itkImageSeriesReader.h>
#include <itkGDCMImageIO.h>
#include "itkQtAdaptor.h" // Per notificar el progrés de lectura
#include "itkImageToVTKImageFilter.h" //Converts an ITK image into a VTK image and plugs a itk data pipeline to a VTK datapipeline.

namespace udg {

/**
    Lector de dades d'imatge per Volume. 
    Aquest lector fa ús de la interfície itkGDCM per llegir les dades.
 */
class VolumePixelDataReaderITKGDCM : public VolumePixelDataReader {
Q_OBJECT
public:
    VolumePixelDataReaderITKGDCM(QObject *parent = 0);
    ~VolumePixelDataReaderITKGDCM();

    int read(const QStringList &filenames);

private:   
    /// Carrega un volum a partir del nom de fitxer que se li passi
    /// @param fileName
    /// @return noError en cas que tot hagi anat bé, el tipus d'error altrament
    int readSingleFile(const QString &fileName);
    
    /// Donades unes imatges que tenen diferents mides, les llegim en un sol
    /// volum adaptant la mida als valors màxims de rows i columns. S'executarà quan volguem llegir
    /// una sèrie que conté imatges de diferents mides.
    /// No deixa de ser un HACK, lo ideal és que cada imatge es llegeixi en volums per separat.
    void readDifferentSizeImagesIntoOneVolume(const QStringList &filenames);

    /// Donat un missatge d'error en un string, ens torna el codi d'error intern que sabem tractar
    int identifyErrorMessage(const QString &errorMessage);

    /// Ens transforma la imatge itk que obtenim de l'output dels readers 
    /// al format vtk que es gestiona internament
    void setData(Volume::ItkImageTypePointer itkImage);

private slots:
    /// Captura el senyal d'itkQtAdaptor perquè poguem notificar el progrés de la lectura
    void slotProgress();

private:
    typedef itk::ImageFileReader<Volume::ItkImageType> ReaderType;
    typedef ReaderType::Pointer ReaderTypePointer;

    typedef itk::ImageSeriesReader<Volume::ItkImageType> SeriesReaderType;
    typedef itk::GDCMImageIO ImageIOType;

    typedef itk::ImageToVTKImageFilter<Volume::ItkImageType> ItkToVtkFilterType;
    
    /// El lector de sèries dicom
    SeriesReaderType::Pointer m_seriesReader;

    /// El lector estàndar de fitxers singulars
    ReaderTypePointer m_reader;

    /// Motor de lectura DICOM de gdcm
    ImageIOType::Pointer m_gdcmIO;

    /// Filtre que ens passa d'itk a vtk
    ItkToVtkFilterType::Pointer m_itkToVtkFilter;
    
    /// TODO membre temporal per la transició al tractament de fases
    int m_numberOfPhases;
    int m_numberOfSlicesPerPhase;

    /// Traductor d'events itk en signals de Qt 
    /// per poder monitorejar el progrés de lectura d'arxius
    itk::QtSignalAdaptor *m_progressSignalAdaptor;
};

} // End namespace udg

#endif
