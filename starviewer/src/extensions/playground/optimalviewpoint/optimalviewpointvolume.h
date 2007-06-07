/***************************************************************************
 *   Copyright (C) 2006 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#ifndef UDGMAGICMIRRORSVOLUME_H
#define UDGMAGICMIRRORSVOLUME_H

#include <QObject>

#include <vector>   // per std::vector<*>

// #include "optimalviewpoint.h" // per OptimalViewpoint::TransferFunction

class vtkColorTransferFunction;
class vtkImageData;
class vtkPiecewiseFunction;
class vtkVolume;
class vtkVolumeProperty;
class vtkVolumeRayCastMapper;
class vtkVolumeRayCastCompositeFunction;
class vtkVolumeRayCastCompositeFunctionOptimalViewpoint;

namespace udg {

class TransferFunction;

/**
 * Encapsula el tractament de vtkVolumes que corresponen a un mateix model:
 * assignar funcions de transferència i sincronitzar les transformacions.
 */
class OptimalViewpointVolume : public QObject {

    Q_OBJECT

public:

    OptimalViewpointVolume( vtkImageData * image );
    ~OptimalViewpointVolume();

    /// Retorna el vtkVolume corresponent a l'índex donat.
    vtkVolume * getMainVolume() const;
    vtkVolume * getPlaneVolume() const;

    void setShade( bool on );

    void setImageSampleDistance( double imageSampleDistance );
    double getImageSampleDistance() const { return m_imageSampleDistance; }

    void setSampleDistance( double sampleDistance );
    double getSampleDistance() const { return m_sampleDistance; }

    /**
     * Estableix la funció de transferència d'opacitat pel vtkVolume
     * corresponent a l'índex donat.
     */
    void setOpacityTransferFunction( vtkPiecewiseFunction * opacityTransferFunction);

    /**
     * Estableix la funció de transferència de color pel vtkVolume corresponent
     * a l'índex donat.
     */
    void setColorTransferFunction( vtkColorTransferFunction * colorTransferFunction);

    /**
     * Sincronitza les tranformacions de tots els vtkVolumes. Concretament,
     * aplica la transformació del vtkVolume amb índex 0 a tots els altres
     * vtkVolumes.
     */
    void synchronize();

    void handle( int rayId, int offset );
    void endRay( int rayId );

    /// Carrega la segmentació del volum des dun fitxer de text.
    signed char loadSegmentationFromFile( const QString & segmentationFileName );
    /// Fa una segmentació automàtica del volum.
    unsigned char segmentateVolume( unsigned short iterations, unsigned char numberOfClusters, double noise );

    void setSegmentationFileName( QString name );




    // nous paràmetres
    void setOpacityForComputing( bool on );
    static const int INTERPOLATION_NEAREST_NEIGHBOUR = 0,
                     INTERPOLATION_LINEAR_INTERPOLATE_CLASSIFY = 1,
                     INTERPOLATION_LINEAR_CLASSIFY_INTERPOLATE = 2;
    void setInterpolation( int interpolation );
    void setSpecular( bool on );
    void setSpecularPower( double specularPower );







    vtkImageData * getLabeledImage() const { return m_simplifiedImage; }




public slots:

    void setExcessEntropy( double excessEntropy );
    void setComputing( bool on = true );

private:

    /// Genera la imatge etiquetada i la segmentada a partir dels limits donats.
    void labelize( const std::vector< unsigned char> & limits );
    /// Genera una funció de transferència ajustada a la segmentació a partir dels límits donats.
    void generateAdjustedTransferFunction( const std::vector< unsigned char> & limits );

    /// Model de vòxels original.
    vtkImageData * m_image;
    vtkImageData * m_simplifiedImage;
    vtkImageData * m_segmentedImage;

    /// Vector de volums.
    vtkVolume * m_mainVolume;
    vtkVolume * m_planeVolume;

    /// Vector de mappers.
    vtkVolumeRayCastMapper * m_mainMapper;
    vtkVolumeRayCastMapper * m_planeMapper;

    /// Vector de funcions de ray-cast.
    vtkVolumeRayCastCompositeFunction * m_mainVolumeRayCastFunction;
    vtkVolumeRayCastCompositeFunctionOptimalViewpoint * m_planeVolumeRayCastFunction;

    /// Vector de funcions de transferència d'opacitat.
    vtkPiecewiseFunction * m_opacityTransferFunction;

    /// Vector de funcions de transferència de color.
    vtkColorTransferFunction * m_colorTransferFunction;

    /// Vector de propietats de volum.
    vtkVolumeProperty * m_volumeProperty;



    unsigned char * m_data;
    unsigned char * m_simplifiedData;
    unsigned char * m_segmentedData;
    int m_dataSize;

    double m_imageSampleDistance;
    double m_sampleDistance;

    double m_excessEntropy;

    QString m_segmentationFileName;


signals:

    void needsExcessEntropy();
    void visited( int rayId, unsigned char value );
    void rayEnd( int rayId );
    void adjustedTransferFunctionDefined( const TransferFunction & adjustedTransferFunction );


}; // end class OptimalViewpointVolume

}; // end namespace udg

#endif // UDGMAGICMIRRORSVOLUME_H
