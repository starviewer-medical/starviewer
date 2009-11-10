#ifndef UDGEXPERIMENTAL3DVOLUME_H
#define UDGEXPERIMENTAL3DVOLUME_H


#include <QVector>

#include "vector3.h"


class vtkEncodedGradientEstimator;
class vtkFiniteDifferenceGradientEstimator;
class vtkImageData;
class vtkVolume;
class vtkVolumeProperty;
class vtkVolumeRayCastCompositeFunction;
class vtkVolumeRayCastMapper;


namespace udg {


class AmbientVoxelShader;
class CelShadingVoxelShader;
class ColorBleedingVoxelShader;
class ContourVoxelShader;
class CoolWarmVoxelShader;
class DirectIlluminationVoxelShader;
class Obscurance;
class ObscuranceVoxelShader;
class TransferFunction;
class Volume;
class vtk4DLinearRegressionGradientEstimator;
class vtkVolumeRayCastVoxelShaderCompositeFunction;

// VMI
class VmiVoxelShader2;
class VomiVoxelShader;
class VomiCoolWarmVoxelShader;
class VoxelSaliencyVoxelShader;
class ColorVomiVoxelShader;
class OpacityVoxelShader;


class Experimental3DVolume {

public:

    /// Tipus d'interpolació.
    enum Interpolation { NearestNeighbour, LinearInterpolateClassify, LinearClassifyInterpolate };
    /// Estimadors de gradient.
    enum GradientEstimator { FiniteDifference, FourDLInearRegression1, FourDLInearRegression2 };

    Experimental3DVolume( Volume *volume );
    Experimental3DVolume( vtkImageData *image );
    ~Experimental3DVolume();

    /// Retorna el model de vòxels principal.
    vtkImageData* getImage() const;
    /// Retorna el vtkVolume.
    vtkVolume* getVolume() const;
    /// Retorna el valor de propietat mínim.
    unsigned short getRangeMin() const;
    /// Retorna el valor de propietat màxim.
    unsigned short getRangeMax() const;
    /// Retorna la mida (nombre de vòxels) del volum.
    unsigned int getSize() const;

    /// Estableix el tipus d'interpolació.
    void setInterpolation( Interpolation interpolation );
    /// Estableix l'estimador de gradient.
    void setGradientEstimator( GradientEstimator gradientEstimator );
    /// Assigna les opcions predeterminades de shading: no es pinta res.
    void resetShadingOptions();
    /// Afegeix il·luminació al shading.
    void addLighting( bool diffuse = false, bool specular = false, double specularPower = 0.0 );
    /// Afegeix cool-warm shading.
    void addCoolWarm( float b, float y, float alpha, float beta );
    /// Afegeix contorn al shading.
    void addContour( double threshold = 0.0 );
    /// Afegeix cel-shading amb el nombre de quantums especificat.
    void addCelShading( int quantums );
    /// Afegeix obscurances al shading.
    void addObscurance( Obscurance *obscurance, double factor, double filterLow, double filterHigh, bool additive = false, double weight = 0.0 );
    /// Estableix la funció de transferència.
    void setTransferFunction( const TransferFunction &transferFunction );

    /// Prepara el rendering amb el voxel shader per fer càlculs de VMI.
    void startVmiMode();
    void startVmiSecondPass();
    QVector<float> finishVmiSecondPass();
    float viewedVolumeInVmiSecondPass() const;
    void addVomi( const QVector<float> &vomi, float maximumVomi, float factor, bool additive = false, float weight = 0.0f );
    void addVomiCoolWarm( const QVector<float> &vomi, float maximumVomi, float factor, float y, float b );
    void addColorVomi( const QVector<Vector3Float> &colorVomi, float maximumColorVomi, float factor );
    void addVoxelSaliencies( const QVector<float> &voxelSaliencies, float maximumSaliency, float factor );
    void addOpacity( const QVector<float> &data, float maximum, float lowThreshold, float lowFactor, float highThreshold, float highFactor );
    QVector<float> computeVomiGradient( const QVector<float> &vomi );

private:

    /// Crea el model de vòxels de treball.
    void createImage( vtkImageData *image );
    /// Crea les volume ray cast functions.
    void createVolumeRayCastFunctions();
    /// Crea els voxel shaders.
    void createVoxelShaders();
    /// Crea el mapper.
    void createMapper();
    /// Crea la propietat.
    void createProperty();
    /// Crea el volum.
    void createVolume();
    /// Retorna l'estimador de gradient actual.
    vtkEncodedGradientEstimator* gradientEstimator() const;

private:

    /// Model de vòxels principal.
    vtkImageData *m_image;

    /// Valor de propietat mínim.
    unsigned short m_rangeMin;
    /// Valor de propietat màxim.
    unsigned short m_rangeMax;

    /// Dades del model de vòxels principal
    unsigned short *m_data;
    /// Mida de les dades.
    unsigned int m_dataSize;

    /// Volume ray cast function principal.
    vtkVolumeRayCastCompositeFunction *m_normalVolumeRayCastFunction;
    /// Volume ray cast function amb shaders.
    vtkVolumeRayCastVoxelShaderCompositeFunction *m_shaderVolumeRayCastFunction;

    /// Voxel shader d'il·luminació ambient.
    AmbientVoxelShader *m_ambientVoxelShader;
    /// Voxel shader d'il·luminació directa (ambient + difusa [+ especular]).
    DirectIlluminationVoxelShader *m_directIlluminationVoxelShader;
    /// Voxel shader de contorns.
    ContourVoxelShader *m_contourVoxelShader;
    /// Voxel shader de cel-shading.
    CelShadingVoxelShader *m_celShadingVoxelShader;
    /// Voxel shader de cool-warm.
    CoolWarmVoxelShader *m_coolWarmVoxelShader;
    /// Voxel shader d'obscurances.
    ObscuranceVoxelShader *m_obscuranceVoxelShader;
    /// Voxel shader de color bleeding.
    ColorBleedingVoxelShader *m_colorBleedingVoxelShader;
    // VMI
    /// Voxel shader per calcular la VMI; serveix per la segona passada.
    VmiVoxelShader2 *m_vmiVoxelShader2;
    /// Voxel shader que pinta les VoMI.
    VomiVoxelShader *m_vomiVoxelShader;
    /// Voxel shader que pinta les VoMI amb cool-warm shading.
    VomiCoolWarmVoxelShader *m_vomiCoolWarmVoxelShader;
    /// Voxel shader que pinta les voxel saliencies.
    VoxelSaliencyVoxelShader *m_voxelSaliencyVoxelShader;
    /// Voxel shader que pinta les color VoMI.
    ColorVomiVoxelShader *m_colorVomiVoxelShader;
    /// Voxel shader que modifica l'opacitat segons la VoMI o la saliency.
    OpacityVoxelShader *m_opacityVoxelShader;

    /// Mapper.
    vtkVolumeRayCastMapper *m_mapper;

    /// Propietat.
    vtkVolumeProperty *m_property;

    /// Volum.
    vtkVolume *m_volume;

    /// Tipus d'estimador de gradient actual.
    GradientEstimator m_gradientEstimator;
    /// Estimador de gradient per diferències finites.
    vtkFiniteDifferenceGradientEstimator *m_finiteDifferenceGradientEstimator;
    /// Estimador de gradient per regressió lineal 4D.
    vtk4DLinearRegressionGradientEstimator *m_4DLinearRegressionGradientEstimator;

};


}


#endif
