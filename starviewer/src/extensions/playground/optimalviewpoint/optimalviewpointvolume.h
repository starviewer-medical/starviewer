/***************************************************************************
 *   Copyright (C) 2006-2008 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.edu/GGG/index.html                                    *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/


#ifndef UDGOPTIMALVIEWPOINTVOLUME_H
#define UDGOPTIMALVIEWPOINTVOLUME_H


#include <QObject>

#include <QHash>
#include <QMutex>
#include <QVector>

#include "transferfunction.h"


class vtkImageData;
class vtkVolume;
class vtkVolumeProperty;
class vtkVolumeRayCastCompositeFunction;
class vtkVolumeRayCastCompositeFunctionFx;
class vtkVolumeRayCastCompositeFunctionObscurances;
class vtkVolumeRayCastCompositeFunctionViewpointSaliency;
class vtkVolumeRayCastMapper;
class vtkRenderer;


namespace udg {


class AmbientVoxelShader;
class ColorBleedingVoxelShader;
class ContourVoxelShader;
class DirectIlluminationVoxelShader;
class ObscuranceVoxelShader;
class SaliencyVoxelShader;
class Vector3;
class vtkVolumeRayCastCompositeFxFunction;


/**
 * Aquesta classe gestiona el tractament de volums de l'extensió Optimal Viewpoint.
 *
 * Guarda tots els objectes necessaris per crear un vtkVolume a partir d'un vtkImageData, permet
 * definir els paràmetres de la visualització del volum amb ray casting, pot segmentar un volum amb
 * les dades d'un fitxer o amb un algorisme propi, etc.
 *
 * Té signals i slots que permeten la comunicació amb OptimalViewpointPlane mentre es fa una
 * visualització.
 *
 * \todo S'hauria d'ampliar una mica més aquesta descripció.
 *
 * \author Grup de Gràfics de Girona (GGG) <vismed@ima.udg.edu>
 */
class OptimalViewpointVolume : public QObject {

    Q_OBJECT

public:

    struct Voxel { int x, y, z; };

    enum ObscuranceFunction { Constant0, Distance, SquareRoot, Exponential, ExponentialNorm, CubeRoot };
    enum ObscuranceVariant { Density, DensitySmooth, Opacity, OpacitySmooth, OpacitySaliency, OpacitySmoothSaliency, OpacityColorBleeding, OpacitySmoothColorBleeding };

    /// Construeix el volum a partir d'una imatge.
    OptimalViewpointVolume( vtkImageData *image, QObject *parent = 0 );
    virtual ~OptimalViewpointVolume();

    /// Retorna el vtkVolume corresponent a l'índex donat.
    vtkVolume * getMainVolume() const;
    vtkVolume * getPlaneVolume() const;

    void setShade( bool on );

    void setImageSampleDistance( double imageSampleDistance );
    double getImageSampleDistance() const;

    void setSampleDistance( double sampleDistance );
    double getSampleDistance() const;

    void setTransferFunction( const TransferFunction & transferFunction );

    /**
     * Estableix la funció de transferència d'opacitat pel vtkVolume
     * corresponent a l'índex donat.
     */
//     void setOpacityTransferFunction( vtkPiecewiseFunction * opacityTransferFunction);

    /**
     * Estableix la funció de transferència de color pel vtkVolume corresponent
     * a l'índex donat.
     */
//     void setColorTransferFunction( vtkColorTransferFunction * colorTransferFunction);

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
    signed char rescale( int bins );

    void setSegmentationFileName( QString name );




    // nous paràmetres
    void setOpacityForComputing( bool on );
    static const int INTERPOLATION_NEAREST_NEIGHBOUR = 0,
                     INTERPOLATION_LINEAR_INTERPOLATE_CLASSIFY = 1,
                     INTERPOLATION_LINEAR_CLASSIFY_INTERPOLATE = 2;
    void setInterpolation( int interpolation );
    void setSpecular( bool on );
    void setSpecularPower( double specularPower );







    vtkImageData * getImage() const { return m_image; }
    vtkImageData * getLabeledImage() const { return m_labeledImage; }

    /// Retorna el valor de propietat mínim.
    unsigned char getRangeMin() const;
    /// Retorna el valor de propietat màxim.
    unsigned char getRangeMax() const;



    void setRenderCluster( bool renderCluster );
    void setClusterLimits( unsigned short first, unsigned short last );



    // synchronized? potser no, si els threads es reparteixen el model sense interseccions
    void handleObscurances( int rayId, int offset );
    void endRayObscurances( int rayId );

    void computeObscurances();
    void computeObscurances2();
    void setObscuranceDirections( int obscuranceDirections );
    void setObscuranceMaximumDistance( double obscuranceMaximumDistance );
    void setObscuranceFunction( ObscuranceFunction obscuranceFunction );
    void setObscuranceVariant( ObscuranceVariant obscuranceVariant );

    void setRenderWithObscurances( bool renderWithObscurances );
    void setObscurancesFactor( double obscurancesFactor );
    void setObscurancesFilters( double obscurancesFilterLow, double obscurancesFilterHigh );

    void computeSaliency();


    void computeViewpointSaliency( int directions, vtkRenderer * renderer, bool divArea );
    void accumulateViewpointSaliency( int threadId, double saliency );

    bool loadObscurances( const QString & obscurancesFileName, bool color);

    void setFx( bool fx );
    void setFxContour( double fxContour );
    void setFxSaliency( bool fxSaliency );
    void setFxSaliencyA( double fxSaliencyA );
    void setFxSaliencyB( double fxSaliencyB );
    void setFxSaliencyLow( double fxSaliencyLow );
    void setFxSaliencyHigh( double fxSaliencyHigh );


    void setMainRenderer( vtkRenderer * renderer ) { m_mainRenderer = renderer; }


public slots:

    void setExcessEntropy( double excessEntropy );
    void setComputing( bool on = true );

    
signals:
    
    void needsExcessEntropy();
    void visited( int rayId, unsigned char value );
    void rayEnd( int rayId );
    void adjustedTransferFunctionDefined( const TransferFunction & adjustedTransferFunction );

private:

    /// Crea els models de vòxels de treball.
    void createImages( vtkImageData *image );
    /// Crea els voxel shaders.
    void createVoxelShaders();
    /// Crea les volume ray cast functions.
    void createVolumeRayCastFunctions();
    /// Crea el mapper.
    void createMapper();
    /// Crea la propietat.
    void createProperty();
    /// Crea el volum.
    void createVolume();


    // definició d'un vòxel
//     struct Voxel { int x, y, z; };

//     QList<Vector3> getLineStarts( int dimX, int dimY, int dimZ, const Vector3 & forward ) const;
//     double obscurance( double distance ) const;
    static void getLineStarts( QVector<Vector3> & lineStarts, int dimX, int dimY, int dimZ, const Vector3 & forward );

    /// Genera la imatge etiquetada i la segmentada a partir dels limits donats.
    void labelize( const QVector< unsigned char > & limits );
    /// Genera una funció de transferència ajustada a la segmentació a partir dels límits donats.
    void generateAdjustedTransferFunction( const QVector< unsigned char > & limits );


    void reduceToHalf();


    /// Model de vòxels principal.
    vtkImageData *m_image;
    /// Model de vòxels etiquetat.
    vtkImageData *m_labeledImage;
    /// Model de vòxels retallat.
    vtkImageData * m_clusterImage;

    /// Valors de propietat mínim i màxim del volum.
    unsigned char m_rangeMin, m_rangeMax;

    /// Array dels valors principals.
    unsigned char *m_data;
    /// Array dels valors etiquetats.
    unsigned char *m_labeledData;
    /// Mida dels arrays de valors.
    int m_dataSize;

    /// Ambient voxel shader.
    AmbientVoxelShader *m_ambientVoxelShader;
    /// Direct illumination voxel shader.
    DirectIlluminationVoxelShader *m_directIlluminationVoxelShader;
    /// Contour voxel shader.
    ContourVoxelShader *m_contourVoxelShader;
    /// Obscurance voxel shader.
    ObscuranceVoxelShader *m_obscuranceVoxelShader;
    /// Color bleeding voxel shader.
    ColorBleedingVoxelShader *m_colorBleedingVoxelShader;
    /// Saliency voxel shader.
    SaliencyVoxelShader *m_saliencyVoxelShader;

    /// Volume ray cast function principal.
    vtkVolumeRayCastCompositeFunction * m_mainVolumeRayCastFunction;
    /// Volume ray cast function per visualitzar amb obscurances.
    vtkVolumeRayCastCompositeFunctionObscurances *m_volumeRayCastFunctionObscurances;
    /// Volume ray cast function per visualitzar amb efectes.
    vtkVolumeRayCastCompositeFunctionFx *m_volumeRayCastFunctionFx;
    /// Volume ray cast function per visualitzar amb efectes.
    vtkVolumeRayCastCompositeFxFunction *m_volumeRayCastFunctionFx2;
    /// Volume ray cast function per calcular la viewpoint saliency.
    vtkVolumeRayCastCompositeFunctionViewpointSaliency *m_volumeRayCastFunctionViewpointSaliency;

    /// Mapper.
    vtkVolumeRayCastMapper *m_mapper;

    /// Propietat.
    vtkVolumeProperty *m_property;

    /// Volum.
    vtkVolume *m_volume;







    vtkRenderer * m_mainRenderer;





    double m_excessEntropy;

    QString m_segmentationFileName;




    bool m_renderCluster;
    bool m_clusterFirst, m_clusterLast;



    double * m_obscurance;
    Vector3 * m_colorBleeding;
    int m_obscuranceDirections;
    double m_obscuranceMaximumDistance;
    ObscuranceFunction m_obscuranceFunction;
    ObscuranceVariant m_obscuranceVariant;

    bool m_renderWithObscurances;

    TransferFunction m_transferFunction;



    // viewpoint saliency
    double * m_saliency;
    QHash<int, double> m_accumulatedViewpointSaliencyPerThread;
    QHash<int, uint> m_pixelsPerThread;
    QMutex m_mutex;


    bool m_fx;
    double m_fxSaliencyA, m_fxSaliencyB;
    double m_fxSaliencyLow, m_fxSaliencyHigh;

};


}


#endif
