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

#include "q3dviewer.h"

#include "abortrendercommand.h"
#include "imageplane.h"
#include "q3dorientationmarker.h"
#include "voilut.h"
#include "volume.h"

#include <QVTKWidget.h>
#include <vtkActor.h>
#include <vtkImageMarchingCubes.h>
#include <vtkMatrix4x4.h>
#include <vtkNew.h>
#include <vtkPlanes.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkSmartVolumeMapper.h>
#include <vtkVolume.h>
#include <vtkVolumeProperty.h>

namespace udg {

Q3DViewer* Q3DViewer::castFromQViewer(QViewer *viewer)
{
    if (!viewer)
    {
        DEBUG_LOG(QString("Cannot cast a null pointer"));
        return nullptr;
    }

    Q3DViewer *viewer3D = qobject_cast<Q3DViewer*>(viewer);

    if (!viewer3D)
    {
        DEBUG_LOG(QString("Q3DViewer casting failed! Provided viewer may not be a Q3DViewer object: %1").arg(viewer->metaObject()->className()));
    }

    return viewer3D;
}

Q3DViewer::Q3DViewer(QWidget *parent)
    : QViewer(parent), m_firstRender(true), m_clippingPlanes(nullptr)
{
    m_vtkWidget->setAutomaticImageCacheEnabled(true);

    vtkNew<AbortRenderCommand> abortRenderCommand;
    getRenderWindow()->AddObserver(vtkCommand::AbortCheckEvent, abortRenderCommand);

    m_orientationMarker = new Q3DOrientationMarker(this->getInteractor(), this);

    m_volumeMapper = vtkSmartVolumeMapper::New();
    m_volumeMapper->InteractiveAdjustSampleDistancesOff();  // Workaround for vtkSmartVolumeMapper bug (https://gitlab.kitware.com/vtk/vtk/issues/17323)
    m_volumeProperty = vtkVolumeProperty::New();
    m_volumeProperty->SetInterpolationTypeToLinear();
    m_vtkVolume = vtkVolume::New();
    m_vtkVolume->SetProperty(m_volumeProperty);
    m_vtkVolume->SetMapper(m_volumeMapper);

    // Creem una funció de transferència per defecte TODO la tenim només per tenir alguna cosa per defecte
    // Opacitat
    m_transferFunction.setOpacity(20.0, 0.0);
    m_transferFunction.setOpacity(255.0, 0.2);
    // Colors
    m_transferFunction.setColor(0.0, 0.0, 0.0, 0.0);
    m_transferFunction.setColor(64.0, 1.0, 0.0, 0.0);
    m_transferFunction.setColor(128.0, 0.0, 0.0, 1.0);
    m_transferFunction.setColor(192.0, 0.0, 1.0, 0.0);
    m_transferFunction.setColor(255.0, 0.0, 0.2, 0.0);

    m_volumeProperty->SetColor(m_transferFunction.vtkColorTransferFunction());
    m_volumeProperty->SetScalarOpacity(m_transferFunction.vtkOpacityTransferFunction());

    m_isosurfaceFilter = vtkImageMarchingCubes::New();
    vtkNew<vtkPolyDataMapper> isosurfaceMapper;
    isosurfaceMapper->SetInputConnection(m_isosurfaceFilter->GetOutputPort());
    m_isosurfaceActor = vtkActor::New();
    m_isosurfaceActor->SetMapper(isosurfaceMapper);

    m_renderer->AddViewProp(m_vtkVolume);
}

Q3DViewer::~Q3DViewer()
{
    m_volumeMapper->Delete();
    m_volumeProperty->Delete();
    m_vtkVolume->Delete();

    m_isosurfaceFilter->Delete();
    m_isosurfaceActor->Delete();

    if (m_clippingPlanes)
    {
        m_clippingPlanes->Delete();
    }
}

VoiLut Q3DViewer::getCurrentVoiLut() const
{
    return m_transferFunction;
}

const TransferFunction& Q3DViewer::getTransferFunction() const
{
    return m_transferFunction;
}

void Q3DViewer::getVolumeBounds(double bounds[6]) const
{
    m_vtkVolume->GetBounds(bounds);
}

void Q3DViewer::setClippingPlanes(vtkPlanes *clippingPlanes)
{
    if (clippingPlanes)
    {
        m_clippingPlanes = clippingPlanes;
        m_clippingPlanes->Register(nullptr);
        m_volumeMapper->SetClippingPlanes(m_clippingPlanes);
        m_isosurfaceActor->GetMapper()->SetClippingPlanes(m_clippingPlanes);
    }
    else
    {
        DEBUG_LOG("Null clipping planes");
    }
}

vtkPlanes* Q3DViewer::getClippingPlanes() const
{
    return m_clippingPlanes;
}

void Q3DViewer::setInput(Volume *volume)
{
    setCursor(Qt::WaitCursor);
    setViewerStatus(VisualizingVolume);

    if (!checkInputVolume(volume))
    {
        unsetCursor();
        return;
    }

    if (m_clippingPlanes)
    {
        m_volumeMapper->RemoveAllClippingPlanes();
        m_isosurfaceActor->GetMapper()->RemoveAllClippingPlanes();
        m_clippingPlanes->Delete();
        m_clippingPlanes = nullptr;
    }

    m_mainVolume = volume;
    m_mainVolume->getVtkData()->Modified(); // Workaround for vtkSmartVolumeMapper bug (https://gitlab.kitware.com/vtk/vtk/issues/17328)
    m_volumeMapper->SetInputData(m_mainVolume->getVtkData());
    m_volumeMapper->SetSampleDistance(-1.0);    // force the mapper to compute a sample distance based on data spacing
    m_isosurfaceFilter->SetInputData(m_mainVolume->getVtkData());

    setVolumeTransformation();

    m_firstRender = true;

    int *dimensions = volume->getDimensions();

    // Just check that it's not a plain image
    m_canEnableShading = dimensions[0] > 1 && dimensions[1] > 1 && dimensions[2] > 1;

    applyCurrentRenderingMethod();

    // Indiquem el canvi de volum
    emit volumeChanged(getMainInput());

    unsetCursor();
}

void Q3DViewer::setBlendMode(const BlendMode &mode)
{
    switch (mode)
    {
        case BlendMode::Composite: m_volumeMapper->SetBlendModeToComposite(); break;
        case BlendMode::MaximumIntensity: m_volumeMapper->SetBlendModeToMaximumIntensity(); break;
        case BlendMode::MinimumIntensity: m_volumeMapper->SetBlendModeToMinimumIntensity(); break;
        case BlendMode::AverageIntensity: m_volumeMapper->SetBlendModeToAverageIntensity(); break;
        case BlendMode::Additive: m_volumeMapper->SetBlendModeToAdditive(); break;
        case BlendMode::Isosurface: break;
    }

    if (mode == BlendMode::Isosurface)
    {
        m_renderer->RemoveViewProp(m_vtkVolume);
        m_renderer->AddViewProp(m_isosurfaceActor);
    }
    else
    {
        m_renderer->RemoveViewProp(m_isosurfaceActor);
        m_renderer->AddViewProp(m_vtkVolume);
    }
}

void Q3DViewer::setRenderMode(const RenderMode &mode)
{
    switch (mode)
    {
        case RenderMode::SmartRayCasting: m_volumeMapper->SetRequestedRenderModeToDefault(); break;
        case RenderMode::CpuRayCasting: m_volumeMapper->SetRequestedRenderModeToRayCast(); break;
        case RenderMode::GpuRayCasting: m_volumeMapper->SetRequestedRenderModeToGPU(); break;
    }
}

void Q3DViewer::setInterpolationMode(const InterpolationMode &mode)
{
    switch (mode)
    {
        case InterpolationMode::NearestNeighbor: m_volumeMapper->SetInterpolationModeToNearestNeighbor(); break;
        case InterpolationMode::Linear: m_volumeMapper->SetInterpolationModeToLinear(); break;
        case InterpolationMode::Cubic: m_volumeMapper->SetInterpolationModeToCubic(); break;
    }
}

void Q3DViewer::setIndependentComponents(bool independent)
{
    m_volumeProperty->SetIndependentComponents(independent);
}

void Q3DViewer::applyCurrentRenderingMethod()
{
    if (m_firstRender)
    {
        setDefaultViewForCurrentInput();
        m_firstRender = false;
    }

    render();
}

void Q3DViewer::setVoiLut(const VoiLut &voiLut)
{
    setTransferFunction(voiLut.getLut());
    this->applyCurrentRenderingMethod();
    emit transferFunctionChanged();
}

void Q3DViewer::setTransferFunction(const TransferFunction &transferFunction)
{
    m_transferFunction = transferFunction;

    m_volumeProperty->SetScalarOpacity(m_transferFunction.vtkOpacityTransferFunction());
    m_volumeProperty->SetColor(m_transferFunction.vtkColorTransferFunction());
}

void Q3DViewer::setShading(bool on)
{
    if (m_canEnableShading && on)
    {
        m_volumeProperty->ShadeOn();
    }
    else
    {
        m_volumeProperty->ShadeOff();
    }
}

void Q3DViewer::setAmbientCoefficient(double ambientCoefficient)
{
    m_volumeProperty->SetAmbient(ambientCoefficient);
}

void Q3DViewer::setDiffuseCoefficient(double diffuseCoefficient)
{
    m_volumeProperty->SetDiffuse(diffuseCoefficient);
}

void Q3DViewer::setSpecularCoefficient(double specularCoefficient)
{
    m_volumeProperty->SetSpecular(specularCoefficient);
}

void Q3DViewer::setSpecularPower(double specularPower)
{
    m_volumeProperty->SetSpecularPower(specularPower);
}

void Q3DViewer::setIsoValue(int isoValue)
{
    m_isosurfaceFilter->SetValue(0, isoValue);
}

void Q3DViewer::enableOrientationMarker(bool enable)
{
    m_orientationMarker->setEnabled(enable);
}

void Q3DViewer::orientationMarkerOn()
{
    this->enableOrientationMarker(true);
}

void Q3DViewer::orientationMarkerOff()
{
    this->enableOrientationMarker(false);
}

void Q3DViewer::getCurrentRenderedItemBounds(double bounds[6])
{
    m_vtkVolume->GetBounds(bounds);
}

bool Q3DViewer::checkInputVolume(Volume *volume)
{
    if (!volume)
    {
        WARN_LOG("Null volume");
        return false;
    }

    if (volume->getNumberOfFrames() == 0)
    {
        WARN_LOG("The volume has no images");
        return false;
    }

    return true;
}

void Q3DViewer::setVolumeTransformation()
{
    Image *imageReference = getMainInput()->getImage(0);
    ImagePlane currentPlane;
    currentPlane.fillFromImage(imageReference);
    std::array<double, 3> currentPlaneRowVector = Vector3(currentPlane.getImageOrientation().getRowVector());
    std::array<double, 3> currentPlaneColumnVector = Vector3(currentPlane.getImageOrientation().getColumnVector());
    double stackDirection[3];
    getMainInput()->getStackDirection(stackDirection);

    DEBUG_LOG(QString("currentPlaneRowVector: %1 %2 %3").arg(currentPlaneRowVector[0]).arg(currentPlaneRowVector[1]).arg(currentPlaneRowVector[2]));
    DEBUG_LOG(QString("currentPlaneColumnVector: %1 %2 %3").arg(currentPlaneColumnVector[0]).arg(currentPlaneColumnVector[1]).arg(currentPlaneColumnVector[2]));
    DEBUG_LOG(QString("stackDirection: %1 %2 %3").arg(stackDirection[0]).arg(stackDirection[1]).arg(stackDirection[2]));

    vtkMatrix4x4 *projectionMatrix = vtkMatrix4x4::New();
    projectionMatrix->Identity();

    if ((   currentPlaneRowVector[0] == 0.0 &&    currentPlaneRowVector[1] == 0.0 &&    currentPlaneRowVector[2] == 0.0) ||
        (currentPlaneColumnVector[0] == 0.0 && currentPlaneColumnVector[1] == 0.0 && currentPlaneColumnVector[2] == 0.0) ||
        (          stackDirection[0] == 0.0 &&           stackDirection[1] == 0.0 &&           stackDirection[2] == 0.0))
    {
        DEBUG_LOG("One of the vectors is null: setting an identity projection matrix.");
    }
    else
    {
        for (int row = 0; row < 3; row++)
        {
            projectionMatrix->SetElement(row, 0, currentPlaneRowVector[row]);
            projectionMatrix->SetElement(row, 1, currentPlaneColumnVector[row]);
            projectionMatrix->SetElement(row, 2, stackDirection[row]);
        }
    }

    m_vtkVolume->SetUserMatrix(projectionMatrix);
    m_isosurfaceActor->SetUserMatrix(projectionMatrix);

    projectionMatrix->Delete();
}

void Q3DViewer::setDefaultViewForCurrentInput()
{
    resetViewToCoronal();
}

}
