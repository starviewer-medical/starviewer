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

#include <vtkMultiThreader.h>

#include "transferfunction.h"


class vtkImageData;
class vtkVolume;
class vtkVolumeProperty;
class vtkVolumeRayCastCompositeFunction;
class vtkVolumeRayCastCompositeFunctionFx;
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
class vtkVolumeRayCastVoxelShaderCompositeFunction;


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

    /// Tipus d'interpolació.
    enum Interpolation { NearestNeighbour, LinearInterpolateClassify, LinearClassifyInterpolate };

    /// Construeix el volum a partir d'una imatge.
    OptimalViewpointVolume( vtkImageData *image, QObject *parent = 0 );
    /// Destructor.
    virtual ~OptimalViewpointVolume();

    /// Retorna la imatge principal.
    vtkImageData* getImage() const;
    /// Retorna la imatge etiquetada.
    vtkImageData* getLabeledImage();
    /// Retorna el vtkVolume.
    vtkVolume* getVolume() const;

    /// Retorna el valor de propietat mínim.
    unsigned char getRangeMin() const;
    /// Retorna el valor de propietat màxim.
    unsigned char getRangeMax() const;

    /// Assigna el renderer.
    void setRenderer( vtkRenderer *renderer );

    /// Assigna la primera i l'última llesca del model retallat.
    void setClusterLimits( unsigned short first, unsigned short last );
    /// Assigna si s'ha de visualitzar el model retallat.
    void setRenderCluster( bool renderCluster );

    /// Assigna la funció de transferència.
    void setTransferFunction( const TransferFunction &transferFunction );
    /// Assigna el tipus d'interpolació que es farà.
    void setInterpolation( Interpolation interpolation );
    /// Assigna si s'aplica ombreig (il·luminació difusa) o no (ambient).
    void setShade( bool on );
    /// Assigna si s'aplica il·luminació especular o no (només té efecte si hi ha il·luminació difusa).
    void setSpecular( bool on );
    /// Assigna l'exponent del coeficient especular.
    void setSpecularPower( double specularPower );

    


    void setImageSampleDistance( double imageSampleDistance );
    double getImageSampleDistance() const;

    void setSampleDistance( double sampleDistance );
    double getSampleDistance() const;



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

    







    void computeObscurances( int numberOfDirections, double maximumDistance, int function, int variant );

    void setRenderWithObscurances( bool renderWithObscurances );
    void setObscurancesFactor( double obscurancesFactor );
    void setObscurancesFilters( double obscurancesFilterLow, double obscurancesFilterHigh );

    void computeSaliency();


    void computeViewpointSaliency( int directions, vtkRenderer * renderer, bool divArea );
    void accumulateViewpointSaliency( vtkMultiThreaderIDType threadId, double saliency );

    bool loadObscurances( const QString & obscurancesFileName, bool color);

    void setFx( bool fx );
    void setFxContour( double fxContour );
    void setFxSaliency( bool fxSaliency );
    void setFxSaliencyA( double fxSaliencyA );
    void setFxSaliencyB( double fxSaliencyB );
    void setFxSaliencyLow( double fxSaliencyLow );
    void setFxSaliencyHigh( double fxSaliencyHigh );


   


public slots:

    void setExcessEntropy( double excessEntropy );
    void setComputing( bool on = true );

    
signals:
    
    void needsExcessEntropy();
    void visited( int rayId, unsigned char value );
    void rayEnd( int rayId );
    void adjustedTransferFunctionDefined( const TransferFunction & adjustedTransferFunction );
    void finishedObscurances();

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
    /// Crea la imatge etiquetada.
    void createLabeledImage();


    // definició d'un vòxel
//     struct Voxel { int x, y, z; };

//     QList<Vector3> getLineStarts( int dimX, int dimY, int dimZ, const Vector3 & forward ) const;
//     double obscurance( double distance ) const;
    

    /// Genera la imatge etiquetada i la segmentada a partir dels limits donats.
    void labelize( const QVector< unsigned char > & limits );
    /// Genera una funció de transferència ajustada a la segmentació a partir dels límits donats.
    void generateAdjustedTransferFunction( const QVector< unsigned char > & limits );


    void reduceToHalf();

private slots:

    void endComputeObscurances();

private:


    /// Model de vòxels principal.
    vtkImageData *m_image;
    /// Model de vòxels etiquetat.
    vtkImageData *m_labeledImage;

    /// Model de vòxels retallat.
    vtkImageData *m_clusterImage;
    /// Primera i última llesca del model retallat.
    unsigned short m_clusterFirstSlice, m_clusterLastSlice;

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
    /// Volume ray cast function per visualitzar amb efectes.
    vtkVolumeRayCastCompositeFunctionFx *m_volumeRayCastFunctionFx;
    /// Volume ray cast function per visualitzar amb efectes.
    vtkVolumeRayCastVoxelShaderCompositeFunction *m_volumeRayCastFunctionFx2;
    /// Volume ray cast function per calcular la viewpoint saliency.
    vtkVolumeRayCastCompositeFunctionViewpointSaliency *m_volumeRayCastFunctionViewpointSaliency;

    /// Mapper.
    vtkVolumeRayCastMapper *m_mapper;

    /// Propietat.
    vtkVolumeProperty *m_property;

    /// Volum.
    vtkVolume *m_volume;

    /// Funció de transferència actual.
    TransferFunction m_transferFunction;    // necessària per les obscurances

    /// El renderer on es visualitza el vtkVolume.
    vtkRenderer *m_renderer;    // necessari pel direct illumination voxel shader

    /// Variant de les obscurances que s'ha calculat o carregat.
    int m_obscuranceVariant;
    /// Vector d'obscurances.
    double *m_obscurance;
    /// Vector de color bleeding.
    Vector3 *m_colorBleeding;







    double m_excessEntropy;



    






    bool m_renderWithObscurances;

    



    // viewpoint saliency
    double * m_saliency;
    QHash<vtkMultiThreaderIDType, double> m_accumulatedViewpointSaliencyPerThread;
    QHash<vtkMultiThreaderIDType, uint> m_pixelsPerThread;
    QMutex m_mutex;


    bool m_fx;
    double m_fxSaliencyA, m_fxSaliencyB;
    double m_fxSaliencyLow, m_fxSaliencyHigh;

};


}


#endif
