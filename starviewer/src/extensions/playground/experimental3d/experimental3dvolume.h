#ifndef UDGEXPERIMENTAL3DVOLUME_H
#define UDGEXPERIMENTAL3DVOLUME_H


class vtkImageData;
class vtkVolume;
class vtkVolumeProperty;
class vtkVolumeRayCastCompositeFunction;
class vtkVolumeRayCastMapper;


namespace udg {


class TransferFunction;
class Volume;


class Experimental3DVolume {

public:

    /// Tipus d'interpolació.
    enum Interpolation { NearestNeighbour, LinearInterpolateClassify, LinearClassifyInterpolate };

    Experimental3DVolume( Volume *volume );
    ~Experimental3DVolume();

    /// Retorna el vtkVolume.
    vtkVolume* getVolume() const;
    /// Retorna el valor de propietat mínim.
    unsigned char getRangeMin() const;
    /// Retorna el valor de propietat màxim.
    unsigned char getRangeMax() const;

    /// Estableix el tipus d'interpolació.
    void setInterpolation( Interpolation interpolation );
    /// Estableix els paràmetres d'il·luminació.
    void setLighting( bool diffuse, bool specular, double specularPower );
    /// Estableix la funció de transferència.
    void setTransferFunction( const TransferFunction &transferFunction );

private:

    /// Crea el model de vòxels de treball.
    void createImage( Volume *volume );
    /// Crea la volume ray cast function.
    void createVolumeRayCastFunction();
    /// Crea el mapper.
    void createMapper();
    /// Crea la propietat.
    void createProperty();
    /// Crea el volum.
    void createVolume();

private:

    /// Model de vòxels principal.
    vtkImageData *m_image;

    /// Valor de propietat mínim.
    unsigned char m_rangeMin;
    /// Valor de propietat màxim.
    unsigned char m_rangeMax;

    /// Dades del model de vòxels principal
    unsigned char *m_data;
    /// Mida de les dades.
    unsigned int m_dataSize;

    /// Volume ray cast function principal.
    vtkVolumeRayCastCompositeFunction *m_normalVolumeRayCastFunction;

    /// Mapper.
    vtkVolumeRayCastMapper *m_mapper;

    /// Propietat.
    vtkVolumeProperty *m_property;

    /// Volum.
    vtkVolume *m_volume;

};


}


#endif
