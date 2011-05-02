#ifndef UDGEXPERIMENTAL3DVOLUME_H
#define UDGEXPERIMENTAL3DVOLUME_H

#include "vector3.h"

#include <QVector>

class vtkEncodedGradientEstimator;
class vtkFiniteDifferenceGradientEstimator;
class vtkImageData;
class vtkOpenGLGPUVolumeRayCastMapper;
class vtkVolume;
class vtkVolumeProperty;
class vtkVolumeRayCastCompositeFunction;
class vtkVolumeRayCastMapper;

namespace udg {

class AmbientVoxelShader2;
class CelShadingVoxelShader;
class ColorBleedingVoxelShader;
class ColorVomiVoxelShader;
class ContourVoxelShader;
class CoolWarmVoxelShader;
class DirectIlluminationVoxelShader2;
class FilteringAmbientOcclusionMapVoxelShader;
class FilteringAmbientOcclusionStipplingVoxelShader;
class FilteringAmbientOcclusionVoxelShader;
class ImiVoxelShader;
class Obscurance;
class ObscuranceVoxelShader;
class OpacityVoxelShader;
class TransferFunction;
class VmiVoxelShader2;
class Volume;
class VomiCoolWarmVoxelShader;
class VomiGammaVoxelShader;
class VomiVoxelShader;
class VoxelSaliencyVoxelShader;
class vtk4DLinearRegressionGradientEstimator;
class vtkVolumeRayCastVoxelShaderCompositeFunction;
class WhiteVoxelShader;
class QExperimental3DExtension;

/**
    Aquesta classe agrupa totes les funcions de tractament de volums de l'extensió experimental3d.
  */
class Experimental3DVolume {

public:

    /// Tipus d'interpolació.
    enum Interpolation { NearestNeighbour, LinearInterpolateClassify, LinearClassifyInterpolate };
    /// Estimadors de gradient.
    enum GradientEstimator { FiniteDifference, FourDLInearRegression1, FourDLInearRegression2 };

    Experimental3DVolume(Volume *volume);
    Experimental3DVolume(vtkImageData *image);
    ~Experimental3DVolume();

    /// Assigna el mòdels de vòxels alternatiu.
    /// TODO Això és una marranada per interpolar sobre el volum original en l'optimització de funcions de transferència 2D. Cal pensar com fer-ho ben fet.
    void setAlternativeImage(vtkImageData *alternativeImage);
    /// Assigna l'extensió.
    /// TODO Això és una marranada per interpolar sobre el volum original en l'optimització de funcions de transferència 2D. Cal pensar com fer-ho ben fet.
    void setExtension(const QExperimental3DExtension *extension);

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
    /// Retorna la magnitud del gradient de cada vòxel.
    const unsigned char* gradientMagnitudes() const;

    /// Estableix el tipus d'interpolació.
    void setInterpolation(Interpolation interpolation);
    /// Estableix l'estimador de gradient.
    void setGradientEstimator(GradientEstimator gradientEstimator);
    /// Assigna les opcions predeterminades de shading: no es pinta res.
    void resetShadingOptions();
    /// Afegeix il·luminació ambient al shading.
    void addAmbientLighting();
    /// Afegeix il·luminació completa al shading.
    void addFullLighting(double ambient, double diffuse, double specular, double specularPower);
    /// Afegeix cool-warm shading.
    void addCoolWarm(float b, float y, float alpha, float beta);
    /// Afegeix pintar el volum en blanc.
    void addWhite();
    /// Afegeix contorn al shading.
    void addContour(double threshold = 0.0);
    /// Afegeix cel-shading amb el nombre de quantums especificat.
    void addCelShading(int quantums);
    /// Afegeix obscurances al shading.
    void addObscurance(Obscurance *obscurance, double factor, double filterLow, double filterHigh, bool additive = false, double weight = 0.0);
    /// Estableix la funció de transferència.
    void setTransferFunction(const TransferFunction &transferFunction, bool useGradientOpacityTransferFunction = false);
    /// Força el renderitzat amb CPU encara que es pogués fer amb GPU.
    void forceCpuRendering();
    /// Força el renderitzat amb voxel shaders de CPU encara que es pogués fer amb GPU o amb CPU amb el pipeline normal de VTK.
    void forceCpuShaderRendering();

    /// Prepara el rendering amb el voxel shader per fer càlculs de VMI.
    void startVmiMode();
    void startVmiSecondPass();
    QVector<float> finishVmiSecondPass();
    float viewedVolumeInVmiSecondPass() const;
    void addVomi(const QVector<float> &vomi, float minimumVomi, float maximumVomi, float factor, bool additive = false, float weight = 0.0f);
    void addVomiGamma(const QVector<float> &vomi, float maximumVomi, float factor, float gamma, bool additive = false, float weight = 0.0f);
    void addVomiCoolWarm(const QVector<float> &vomi, float maximumVomi, float factor, float y, float b);
    void addColorVomi(const QVector<Vector3Float> &colorVomi, float maximumColorVomi, float factor);
    void addImi(const QVector<float> &imi, float maximumImi, float factor, bool additive = false, float weight = 0.0f);
    void addVoxelSaliencies(const QVector<float> &voxelSaliencies, float maximumSaliency, float factor);
    void addOpacity(const QVector<float> &data, float maximum, float lowThreshold, float lowFactor, float highThreshold, float highFactor);
    void addOpacity(const QVector<float> &data, float maximum);
    QVector<float> computeVomiGradient(const QVector<float> &vomi);

    void addFilteringAmbientOcclusion(const QVector<float> &filteringAmbientOcclusion, float maximum, float lambda);
    void addFilteringAmbientOcclusionMap(const QVector<float> &filteringAmbientOcclusion, float maximum, float factor);
    void addFilteringAmbientOcclusionStippling(const QVector<float> &filteringAmbientOcclusion, float maximum, float threshold, float factor);

private:

    /// Crea el model de vòxels de treball.
    void createImage(vtkImageData *image);
    /// Crea les volume ray cast functions.
    void createVolumeRayCastFunctions();
    /// Crea els voxel shaders.
    void createVoxelShaders();
    /// Crea els mappers.
    void createMappers();
    /// Crea la propietat.
    void createProperty();
    /// Crea el volum.
    void createVolume();
    /// Retorna l'estimador de gradient actual.
    vtkEncodedGradientEstimator* gradientEstimator() const;

private:

    /// Model de vòxels principal.
    vtkImageData *m_image;
    /// Model de vòxels alternatiu.
    /// TODO Això és una marranada per interpolar sobre el volum original en l'optimització de funcions de transferència 2D. Cal pensar com fer-ho ben fet.
    vtkImageData *m_alternativeImage;

    /// L'extensió.
    /// TODO Això és una marranada per interpolar sobre el volum original en l'optimització de funcions de transferència 2D. Cal pensar com fer-ho ben fet.
    const QExperimental3DExtension *m_extension;

    /// Valor de propietat mínim.
    unsigned short m_rangeMin;
    /// Valor de propietat màxim.
    unsigned short m_rangeMax;

    /// Dades del model de vòxels principal
    unsigned short *m_data;
    /// Mida de les dades.
    unsigned int m_dataSize;

    /// Volume ray cast function pels renderings més senzills que no es poden fer amb GPU.
    /// Això és pel cas especial que fem servir la funció de transferència d'opacitat del gradient,
    /// que de moment (5.6) no està suportat pel ray cast mapper de GPU.
    vtkVolumeRayCastCompositeFunction *m_simpleVolumeRayCastFunction;
    /// Volume ray cast function amb shaders.
    vtkVolumeRayCastVoxelShaderCompositeFunction *m_shaderVolumeRayCastFunction;

    /// Voxel shader d'il·luminació ambient.
    AmbientVoxelShader2 *m_ambientVoxelShader;
    /// Voxel shader d'il·luminació directa (ambient + difusa [+ especular]).
    DirectIlluminationVoxelShader2 *m_directIlluminationVoxelShader;
    /// Voxel shader de contorns.
    ContourVoxelShader *m_contourVoxelShader;
    /// Voxel shader de cel-shading.
    CelShadingVoxelShader *m_celShadingVoxelShader;
    /// Voxel shader de cool-warm.
    CoolWarmVoxelShader *m_coolWarmVoxelShader;
    /// Voxel shader blanc.
    WhiteVoxelShader *m_whiteVoxelShader;
    /// Voxel shader d'obscurances.
    ObscuranceVoxelShader *m_obscuranceVoxelShader;
    /// Voxel shader de color bleeding.
    ColorBleedingVoxelShader *m_colorBleedingVoxelShader;
    // VMI
    /// Voxel shader per calcular la VMI; serveix per la segona passada.
    VmiVoxelShader2 *m_vmiVoxelShader2;
    /// Voxel shader que pinta les VoMI.
    VomiVoxelShader *m_vomiVoxelShader;
    /// Voxel shader que pinta les VoMI amb correcció gamma.
    VomiGammaVoxelShader *m_vomiGammaVoxelShader;
    /// Voxel shader que pinta les VoMI amb cool-warm shading.
    VomiCoolWarmVoxelShader *m_vomiCoolWarmVoxelShader;
    /// Voxel shader que pinta les voxel saliencies.
    VoxelSaliencyVoxelShader *m_voxelSaliencyVoxelShader;
    /// Voxel shader que pinta les color VoMI.
    ColorVomiVoxelShader *m_colorVomiVoxelShader;
    /// Voxel shader que modifica l'opacitat segons la VoMI o la saliency.
    OpacityVoxelShader *m_opacityVoxelShader;
    /// Voxel shader que pinta les IMI.
    ImiVoxelShader *m_imiVoxelShader;

    FilteringAmbientOcclusionVoxelShader *m_filteringAmbientOcclusionVoxelShader;
    FilteringAmbientOcclusionMapVoxelShader *m_filteringAmbientOcclusionMapVoxelShader;
    FilteringAmbientOcclusionStipplingVoxelShader *m_filteringAmbientOcclusionStipplingVoxelShader;

    /// Mapper per fer ray casting amb CPU.
    vtkVolumeRayCastMapper *m_cpuRayCastMapper;
    /// Mapper per fer ray casting amb GPU.
    vtkOpenGLGPUVolumeRayCastMapper *m_gpuRayCastMapper;

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

} // namespace udg

#endif // UDGEXPERIMENTAL3DVOLUME_H
