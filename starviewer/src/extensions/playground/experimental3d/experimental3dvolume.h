#ifndef UDGEXPERIMENTAL3DVOLUME_H
#define UDGEXPERIMENTAL3DVOLUME_H


class vtkEncodedGradientEstimator;
class vtkFiniteDifferenceGradientEstimator;
class vtkImageData;
class vtkVolume;
class vtkVolumeProperty;
class vtkVolumeRayCastCompositeFunction;
class vtkVolumeRayCastMapper;


namespace udg {


class AmbientVoxelShader;
class ColorBleedingVoxelShader;
class ContourVoxelShader;
class DirectIlluminationVoxelShader;
class Obscurance;
class ObscuranceVoxelShader;
class TransferFunction;
class Volume;
class vtk4DLinearRegressionGradientEstimator;
class vtkVolumeRayCastVoxelShaderCompositeFunction;


class Experimental3DVolume {

public:

    /// Tipus d'interpolació.
    enum Interpolation { NearestNeighbour, LinearInterpolateClassify, LinearClassifyInterpolate };
    /// Estimadors de gradient.
    enum GradientEstimator { FiniteDifference, FourDLInearRegression1, FourDLInearRegression2 };

    Experimental3DVolume( Volume *volume );
    ~Experimental3DVolume();

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
    /// Estableix els paràmetres d'il·luminació.
    void setLighting( bool diffuse, bool specular, double specularPower );
    /// Estableix els paràmetres del contorn.
    void setContour( bool on, double threshold );
    /// Estableix els paràmetres de les obscurances.
    void setObscurance( bool on, Obscurance *obscurance, double factor, double filterLow, double filterHigh );
    /// Estableix la funció de transferència.
    void setTransferFunction( const TransferFunction &transferFunction );

private:

    /// Crea el model de vòxels de treball.
    void createImage( Volume *volume );
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
    /// Voxel shader d'obscurances.
    ObscuranceVoxelShader *m_obscuranceVoxelShader;
    /// Voxel shader de color bleeding.
    ColorBleedingVoxelShader *m_colorBleedingVoxelShader;

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
