/*************************************************************************************
  Copyright (C) 2014 Laboratori de Gràfics i Imatge, Universitat de Girona &
  Institut de Diagnòstic per la Imatge.
  Girona 2014. All rights reserved.
  http://starviewer.udg.edu

  This file is part of the Starviewer (Medical Imaging Software) open source project.
  It is subject to the license terms in the LICENSE file found in the top-level
  directory of this distribution and at http://starviewer.udg.edu/license. No part of
  the Starviewer (Medical Imaging Software) open source project, including this file,
  may be copied, modified, propagated, or distributed except according to the
  terms contained in the LICENSE file.
 *************************************************************************************/

#ifndef UDGQ3DVIEWER_H
#define UDGQ3DVIEWER_H

#include "qviewer.h"

#include "combiningvoxelshader.h"
#include "transferfunction.h"
#include "vtkVolumeRayCastSingleVoxelShaderCompositeFunction.h"

// FWD declarations

class vtkImageData;
class vtkOpenGLGPUVolumeRayCastMapper;
class vtkVolume;
class vtkVolumeProperty;
class vtkVolumeRayCastCompositeFunction;
class vtkVolumeRayCastIsosurfaceFunction;
class vtkVolumeRayCastMapper;

namespace udg {

// FWD declarations
class Volume;
class Q3DOrientationMarker;
class ObscuranceMainThread;
class AmbientVoxelShader;
class DirectIlluminationVoxelShader;
class ObscuranceVoxelShader;
class Vtk4DLinearRegressionGradientEstimator;
class Obscurance;
class ContourVoxelShader;

/**
    Classe base per als visualitzadors 3D
  */
class Q3DViewer : public QViewer {
Q_OBJECT
public:
    enum RenderFunction { RayCasting, RayCastingObscurance, GpuRayCasting,
                          MIP3D, IsoSurface, Contouring };
    enum ObscuranceQuality { Low, Medium, High };

    Q3DViewer(QWidget *parent = 0);
    ~Q3DViewer();

    /// Retorna el tipus de visualització que es té assignat
    RenderFunction getRenderFunction() const
    {
        return m_renderFunction;
    }

    /// Retorna el tipus de visualització que es té assignat com a un string
    QString getRenderFunctionAsString();

    /// Returns the VOI LUT that is currently applied to the image in this viewer.
    virtual VoiLut getCurrentVoiLut() const;

    /// Determina la bounding box que definex els plans de tall del volum
    void setClippingPlanes(vtkPlanes *clippingPlanes);
    /// Obté els plans de tall que s'han definit sobre el volum
    vtkPlanes* getClippingPlanes() const;

    /// Retorna els bounds del vtkVolume.
    void getVolumeBounds(double bounds[6]) const;

    /// Casts the given QViewer to a Q3DViewer object
    /// If casting is successful, casted pointer to Q3DViewer will be returned, null otherwise
    static Q3DViewer* castFromQViewer(QViewer *viewer);

    /// Returns the current transfer function.
    const TransferFunction& getTransferFunction() const;

public slots:
    virtual void setInput(Volume* volume);

    /// Assignem el tipus de visualització 3D que volem. RayCasting, MIP, reconstrucció de superfícies...
    void setRenderFunction(RenderFunction function);
    void setRenderFunctionToRayCasting();
    void setRenderFunctionToRayCastingObscurance();
    void setRenderFunctionToGpuRayCasting();
    void setRenderFunctionToMIP3D();
    void setRenderFunctionToIsoSurface();
    void setRenderFunctionToContouring();

    /// Mètodes per controlar la visibilitat de l'orientation marker widget
    void enableOrientationMarker(bool enable);
    void orientationMarkerOn();
    void orientationMarkerOff();

    /// Aplica el mètode de rendering actual.
    void applyCurrentRenderingMethod();

    /// Sets the current transfer function.
    void setTransferFunction(const TransferFunction &transferFunction);

    /// Sets the VOI LUT for this viewer.
    virtual void setVoiLut(const VoiLut &voiLut);

    /// Paràmetres d'il·luminació.
    void setShading(bool on);
    void setAmbientCoefficient(double ambientCoefficient);
    void setDiffuseCoefficient(double diffuseCoefficient);
    void setSpecularCoefficient(double specularCoefficient);
    void setSpecularPower(double specularPower);

    /// Paràmetres de contorns.
    void setContour(bool on);
    void setContourThreshold(double threshold);

    /// Càlcul d'obscurances.
    void computeObscurance(ObscuranceQuality quality);
    void cancelObscurance();

    /// Paràmetres d'obscurances.
    void setObscurance(bool on);
    void setObscuranceFactor(double factor);

    /// Paràmetres d'isosuperfícies.
    void setIsoValue(int isoValue);

signals:
    /// TODO documentar els signals
    void obscuranceProgress(int progress);
    void obscuranceComputed();
    /// Es llança quan les obscurances són cancel·lades pel programa (no per l'usuari).
    void obscuranceCancelledByProgram();
    /// Informa del rang de valors del volum quan aquest canvia.
    void scalarRange(double min, double max);
    void transferFunctionChanged();

protected:
    void getCurrentRenderedItemBounds(double bounds[6]);

private:
    /// Computes and sets the transformation matrix for the volume to convert from the DICOM space to the world space.
    void setVolumeTransformation();

    /// Fa la visualització per raycasting
    void renderRayCasting();

    /// Fa la visualització per raycasting amb obscurances
    void renderRayCastingObscurance();

    /// Fa la visualització per ray casting amb GPU.
    void renderGpuRayCasting();

    /// Fa la visualització per contouring
    void renderContouring();

    /// Fa la visualització per MIP3D
    void renderMIP3D();

    /// Fa la visualització per reconstrucció de superfíces
    void renderIsoSurface();

    /// Reescala les dades de volume en el format adequat per als corresponents algorismes. Retorna fals si no pot crear el volum reescalat.
    bool rescale(Volume *volume);

    /// S'encarrega de decidir quina és la millor orientació
    /// depenent del tipus d'input que tenim. Generalment serà
    /// Coronal, però depenent del tipus de Sèrie podria ser una altra
    void setDefaultViewForCurrentInput();

    /// Comprova si el volum és vàlid com a input. Retorna cert si és així, fals altrament
    bool checkInputVolume(Volume *volume);

    /// Retorna cert si és possible reservar la quantitat de memòria especificada.
    bool canAllocateMemory(int size);

    /// Retorna cert si és el volum que volem visualitzar té un format compatible.
    bool isSupportedVolume(Volume *volume);

private slots:
    // TODO falta documentar el mètode
    void endComputeObscurance();

protected:
    /// La funció que es fa servir pel rendering
    RenderFunction m_renderFunction;

private:
    /// La imatge d'entrada dels mappers.
    vtkImageData *m_imageData;

    /// Widget per veure la orientació en 3D
    Q3DOrientationMarker *m_orientationMarker;

    /// El prop 3D de vtk per representar el volum
    vtkVolume *m_vtkVolume;

    /// Propietats que defineixen com es renderitzarà el volum
    vtkVolumeProperty *m_volumeProperty;

    /// Mapper del volum.
    vtkVolumeRayCastMapper *m_volumeMapper;
    /// Mapper de ray casting amb GPU.
    vtkOpenGLGPUVolumeRayCastMapper *m_gpuRayCastMapper;

    /// Voxel shaders.
    AmbientVoxelShader *m_ambientVoxelShader;
    DirectIlluminationVoxelShader *m_directIlluminationVoxelShader;
    ContourVoxelShader *m_contourVoxelShader;
    typedef CombiningVoxelShader<AmbientVoxelShader, ContourVoxelShader> AmbientContourVoxelShader;
    AmbientContourVoxelShader *m_ambientContourVoxelShader;
    typedef CombiningVoxelShader<DirectIlluminationVoxelShader, ContourVoxelShader> DirectIlluminationContourVoxelShader;
    DirectIlluminationContourVoxelShader *m_directIlluminationContourVoxelShader;
    ObscuranceVoxelShader *m_obscuranceVoxelShader;
    typedef CombiningVoxelShader<AmbientVoxelShader, ObscuranceVoxelShader> AmbientObscuranceVoxelShader;
    AmbientObscuranceVoxelShader *m_ambientObscuranceVoxelShader;
    typedef CombiningVoxelShader<DirectIlluminationVoxelShader, ObscuranceVoxelShader> DirectIlluminationObscuranceVoxelShader;
    DirectIlluminationObscuranceVoxelShader *m_directIlluminationObscuranceVoxelShader;
    typedef CombiningVoxelShader<AmbientContourVoxelShader, ObscuranceVoxelShader> AmbientContourObscuranceVoxelShader;
    AmbientContourObscuranceVoxelShader *m_ambientContourObscuranceVoxelShader;
    typedef CombiningVoxelShader<DirectIlluminationContourVoxelShader, ObscuranceVoxelShader> DirectIlluminationContourObscuranceVoxelShader;
    DirectIlluminationContourObscuranceVoxelShader *m_directIlluminationContourObscuranceVoxelShader;

    /// Funcions de ray cast.
    vtkVolumeRayCastCompositeFunction *m_volumeRayCastFunction;
    vtkVolumeRayCastSingleVoxelShaderCompositeFunction<AmbientContourVoxelShader> *m_volumeRayCastAmbientContourFunction;
    vtkVolumeRayCastSingleVoxelShaderCompositeFunction<DirectIlluminationContourVoxelShader> *m_volumeRayCastDirectIlluminationContourFunction;
    vtkVolumeRayCastSingleVoxelShaderCompositeFunction<AmbientObscuranceVoxelShader> *m_volumeRayCastAmbientObscuranceFunction;
    vtkVolumeRayCastSingleVoxelShaderCompositeFunction<DirectIlluminationObscuranceVoxelShader> *m_volumeRayCastDirectIlluminationObscuranceFunction;
    vtkVolumeRayCastSingleVoxelShaderCompositeFunction<AmbientContourObscuranceVoxelShader> *m_volumeRayCastAmbientContourObscuranceFunction;
    vtkVolumeRayCastSingleVoxelShaderCompositeFunction<DirectIlluminationContourObscuranceVoxelShader> *m_volumeRayCastDirectIlluminationContourObscuranceFunction;
    vtkVolumeRayCastIsosurfaceFunction *m_volumeRayCastIsosurfaceFunction;

    /// Current transfer function.
    TransferFunction m_transferFunction;

    /// Booleà per saber si estem fent el primer render (per reiniciar l'orientació).
    bool m_firstRender;

    /// Booleà que indica si es pot activar el shading (si no hi ha cap dimensió igual a 1 i hi ha prou memòria).
    bool m_canEnableShading;

    /// Thread de control del càlcul d'obscurances.
    ObscuranceMainThread *m_obscuranceMainThread;

    /// Booleà que indica si els contorns estan activats.
    bool m_contourOn;

    /// Obscurances.
    Obscurance *m_obscurance;
    /// Booleà que indica si les obscurances estan activades.
    bool m_obscuranceOn;

    /// Range length of the viewed volume.
    double m_range;

    /// Estimador de gradient que farem servir per les obscurances (i per la resta després de calcular les obscurances).
    Vtk4DLinearRegressionGradientEstimator *m_4DLinearRegressionGradientEstimator;

    /// Plans de tall
    vtkPlanes *m_clippingPlanes;
};

};  // End namespace udg

#endif
