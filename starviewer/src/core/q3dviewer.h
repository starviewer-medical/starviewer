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

#include "transferfunction.h"

class vtkActor;
class vtkImageData;
class vtkImageMarchingCubes;
class vtkPlanes;
class vtkSmartVolumeMapper;
class vtkVolume;
class vtkVolumeProperty;

namespace udg {

class Q3DOrientationMarker;
class Volume;

/**
 * @brief The Q3DViewer class renders a volume in 3 dimensions using ray casting.
 */
class Q3DViewer : public QViewer {

    Q_OBJECT

public:
    /// Blend mode: how the samples are combined to create a pixel.
    enum class BlendMode { Composite, MaximumIntensity, MinimumIntensity, AverageIntensity, Additive, Isosurface };
    /// Render mode: what kind of mapper is used.
    enum class RenderMode { SmartRayCasting, CpuRayCasting, GpuRayCasting };
    /// Interpolation mode: how pixels are interpolated when rendering in low resolution.
    enum class InterpolationMode { NearestNeighbor, Linear, Cubic };

    /// Casts the given QViewer to a Q3DViewer object
    /// If casting is successful, casted pointer to Q3DViewer will be returned, null otherwise
    static Q3DViewer* castFromQViewer(QViewer *viewer);

    explicit Q3DViewer(QWidget *parent = nullptr);
    ~Q3DViewer();

    /// Returns the VOI LUT that is currently applied to the image in this viewer.
    VoiLut getCurrentVoiLut() const override;
    /// Returns the current transfer function.
    const TransferFunction& getTransferFunction() const;

    /// Retorna els bounds del vtkVolume.
    void getVolumeBounds(double bounds[6]) const;

    /// Determina la bounding box que definex els plans de tall del volum
    void setClippingPlanes(vtkPlanes *clippingPlanes);
    /// Obté els plans de tall que s'han definit sobre el volum
    vtkPlanes* getClippingPlanes() const;


public slots:
    void setInput(Volume* volume) override;

    /// Sets the blend mode to use.
    void setBlendMode(const BlendMode &mode);
    /// Sets the render mode to use.
    void setRenderMode(const RenderMode &mode);
    /// Sets the interpolation mode to use.
    void setInterpolationMode(const InterpolationMode &mode);

    /// Sets the "independent components" property of the volume. The default is true and this should be set to false for a color image.
    /// In a 4-component image with non-independent components, the first 3 represent RGB and the fourth will be mapped through the opacity transfer function.
    void setIndependentComponents(bool independent);

    /// Aplica el mètode de rendering actual.
    void applyCurrentRenderingMethod();

    /// Sets the VOI LUT for this viewer.
    void setVoiLut(const VoiLut &voiLut) override;
    /// Sets the current transfer function.
    void setTransferFunction(const TransferFunction &transferFunction);

    /// Paràmetres d'il·luminació.
    void setShading(bool on);
    void setAmbientCoefficient(double ambientCoefficient);
    void setDiffuseCoefficient(double diffuseCoefficient);
    void setSpecularCoefficient(double specularCoefficient);
    void setSpecularPower(double specularPower);

    /// Paràmetres d'isosuperfícies.
    void setIsoValue(int isoValue);

    /// Mètodes per controlar la visibilitat de l'orientation marker widget
    void enableOrientationMarker(bool enable);
    void orientationMarkerOn();
    void orientationMarkerOff();

signals:
    void transferFunctionChanged();

protected:
    void getCurrentRenderedItemBounds(double bounds[6]) override;

private:
    /// Comprova si el volum és vàlid com a input. Retorna cert si és així, fals altrament
    bool checkInputVolume(Volume *volume);

    /// Computes and sets the transformation matrix for the volume to convert from the DICOM space to the world space.
    void setVolumeTransformation();

    /// S'encarrega de decidir quina és la millor orientació
    /// depenent del tipus d'input que tenim. Generalment serà
    /// Coronal, però depenent del tipus de Sèrie podria ser una altra
    void setDefaultViewForCurrentInput();

private:
    /// The main mapper for volume rendering.
    vtkSmartVolumeMapper *m_volumeMapper;
    /// Properties of volume rendering.
    vtkVolumeProperty *m_volumeProperty;
    /// The volume actor.
    vtkVolume *m_vtkVolume;

    /// The filter to compute isosurfaces.
    vtkImageMarchingCubes *m_isosurfaceFilter;
    /// The actor for isosurfaces.
    vtkActor *m_isosurfaceActor;

    /// Widget per veure la orientació en 3D
    Q3DOrientationMarker *m_orientationMarker;

    /// Current transfer function.
    TransferFunction m_transferFunction;

    /// Booleà per saber si estem fent el primer render (per reiniciar l'orientació).
    bool m_firstRender;

    /// Booleà que indica si es pot activar el shading (si no hi ha cap dimensió igual a 1 i hi ha prou memòria).
    bool m_canEnableShading;

    /// Plans de tall
    vtkPlanes *m_clippingPlanes;

};

}

#endif
