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
#include "volume.h"
#include "image.h"
#include "series.h"
#include "imageplane.h"
#include "logging.h"
#include "q3dorientationmarker.h"
#include "transferfunction.h"
#include "coresettings.h"

// Include's qt
#include <QString>
#include <QMessageBox>

// Include's vtk

// Pel setAutomaticImageCacheEnabled
#include <QVTKWidget.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
// Rendering 3D
#include <vtkVolumeProperty.h>
#include <vtkVolume.h>
// Ray Cast
#include <vtkVolumeRayCastMapper.h>
#include <vtkVolumeRayCastCompositeFunction.h>
#include <vtkOpenGLGPUVolumeRayCastMapper.h>
// MIP
#include <vtkVolumeRayCastMIPFunction.h>
#include <vtkFiniteDifferenceGradientEstimator.h>
// Contouring
#include <vtkPolyDataMapper.h>
#include <vtkContourFilter.h>
#include <vtkReverseSense.h>
#include <vtkImageShrink3D.h>
#include <vtkImageGaussianSmooth.h>
#include <vtkProperty.h>
#include <vtkDecimatePro.h>
// IsoSurface
#include <vtkVolumeRayCastIsosurfaceFunction.h>
// LUT's
#include <vtkColorTransferFunction.h>
#include <vtkPiecewiseFunction.h>
// Casting
#include <vtkImageShiftScale.h>
// Reorientació del volum
#include <vtkMatrix4x4.h>
// Clippping Planes
#include <vtkPlanes.h>
// Obscurances
#include "obscurancemainthread.h"
#include "ambientvoxelshader.h"
#include "directilluminationvoxelshader.h"
#include "obscurancevoxelshader.h"
#include "contourvoxelshader.h"
#include "vtk4dlinearregressiongradientestimator.h"
#include <vtkPointData.h>
#include <vtkEncodedGradientShader.h>

// Avortar render
#include "abortrendercommand.h"

namespace udg {

Q3DViewer::Q3DViewer(QWidget *parent)
 : QViewer(parent), m_imageData(0), m_vtkVolume(0), m_volumeProperty(0), m_clippingPlanes(0)
{
    m_defaultFitIntoViewportMarginRate = 0.4;
    m_vtkWidget->setAutomaticImageCacheEnabled(true);
    // Avortar render
    AbortRenderCommand *abortRenderCommand = AbortRenderCommand::New();
    getRenderWindow()->AddObserver(vtkCommand::AbortCheckEvent, abortRenderCommand);
    abortRenderCommand->Delete();

    // Per defecte
    m_renderFunction = RayCasting;

    m_orientationMarker = new Q3DOrientationMarker(this->getInteractor(), this);

    // Creem el pipeline del volum
    m_vtkVolume = vtkVolume::New();
    m_volumeProperty = vtkVolumeProperty::New();
    m_volumeProperty->SetInterpolationTypeToLinear();
    m_vtkVolume->SetProperty(m_volumeProperty);
    m_volumeMapper = vtkVolumeRayCastMapper::New();
    m_gpuRayCastMapper = vtkOpenGLGPUVolumeRayCastMapper::New();
    m_vtkVolume->SetMapper(m_volumeMapper);
    m_renderer->AddViewProp(m_vtkVolume);

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

    m_ambientVoxelShader = new AmbientVoxelShader();
    m_directIlluminationVoxelShader = new DirectIlluminationVoxelShader();
    m_contourVoxelShader = new ContourVoxelShader();
    m_obscuranceVoxelShader = new ObscuranceVoxelShader();
    m_ambientContourVoxelShader = new AmbientContourVoxelShader();
    m_ambientContourVoxelShader->setVoxelShaders(m_ambientVoxelShader, m_contourVoxelShader);
    m_directIlluminationContourVoxelShader = new DirectIlluminationContourVoxelShader();
    m_directIlluminationContourVoxelShader->setVoxelShaders(m_directIlluminationVoxelShader, m_contourVoxelShader);
    m_ambientObscuranceVoxelShader = new AmbientObscuranceVoxelShader();
    m_ambientObscuranceVoxelShader->setVoxelShaders(m_ambientVoxelShader, m_obscuranceVoxelShader);
    m_directIlluminationObscuranceVoxelShader = new DirectIlluminationObscuranceVoxelShader();
    m_directIlluminationObscuranceVoxelShader->setVoxelShaders(m_directIlluminationVoxelShader, m_obscuranceVoxelShader);
    m_ambientContourObscuranceVoxelShader = new AmbientContourObscuranceVoxelShader();
    m_ambientContourObscuranceVoxelShader->setVoxelShaders(m_ambientContourVoxelShader, m_obscuranceVoxelShader);
    m_directIlluminationContourObscuranceVoxelShader = new DirectIlluminationContourObscuranceVoxelShader();
    m_directIlluminationContourObscuranceVoxelShader->setVoxelShaders(m_directIlluminationContourVoxelShader, m_obscuranceVoxelShader);

    m_volumeRayCastFunction = vtkVolumeRayCastCompositeFunction::New();
    m_volumeRayCastFunction->SetCompositeMethodToClassifyFirst();
    m_volumeRayCastAmbientContourFunction = vtkVolumeRayCastSingleVoxelShaderCompositeFunction<AmbientContourVoxelShader>::New();
    m_volumeRayCastAmbientContourFunction->SetCompositeMethodToClassifyFirst();
    m_volumeRayCastAmbientContourFunction->SetVoxelShader(m_ambientContourVoxelShader);
    m_volumeRayCastDirectIlluminationContourFunction = vtkVolumeRayCastSingleVoxelShaderCompositeFunction<DirectIlluminationContourVoxelShader>::New();
    m_volumeRayCastDirectIlluminationContourFunction->SetCompositeMethodToClassifyFirst();
    m_volumeRayCastDirectIlluminationContourFunction->SetVoxelShader(m_directIlluminationContourVoxelShader);
    m_volumeRayCastAmbientObscuranceFunction = vtkVolumeRayCastSingleVoxelShaderCompositeFunction<AmbientObscuranceVoxelShader>::New();
    m_volumeRayCastAmbientObscuranceFunction->SetCompositeMethodToClassifyFirst();
    m_volumeRayCastAmbientObscuranceFunction->SetVoxelShader(m_ambientObscuranceVoxelShader);
    m_volumeRayCastDirectIlluminationObscuranceFunction = vtkVolumeRayCastSingleVoxelShaderCompositeFunction<DirectIlluminationObscuranceVoxelShader>::New();
    m_volumeRayCastDirectIlluminationObscuranceFunction->SetCompositeMethodToClassifyFirst();
    m_volumeRayCastDirectIlluminationObscuranceFunction->SetVoxelShader(m_directIlluminationObscuranceVoxelShader);
    m_volumeRayCastAmbientContourObscuranceFunction = vtkVolumeRayCastSingleVoxelShaderCompositeFunction<AmbientContourObscuranceVoxelShader>::New();
    m_volumeRayCastAmbientContourObscuranceFunction->SetCompositeMethodToClassifyFirst();
    m_volumeRayCastAmbientContourObscuranceFunction->SetVoxelShader(m_ambientContourObscuranceVoxelShader);
    m_volumeRayCastDirectIlluminationContourObscuranceFunction =
        vtkVolumeRayCastSingleVoxelShaderCompositeFunction<DirectIlluminationContourObscuranceVoxelShader>::New();
    m_volumeRayCastDirectIlluminationContourObscuranceFunction->SetCompositeMethodToClassifyFirst();
    m_volumeRayCastDirectIlluminationContourObscuranceFunction->SetVoxelShader(m_directIlluminationContourObscuranceVoxelShader);
    m_volumeRayCastIsosurfaceFunction = vtkVolumeRayCastIsosurfaceFunction::New();

    m_contourOn = false;

    m_firstRender = true;
    m_obscuranceMainThread = 0;
    m_obscurance = 0;
    m_obscuranceOn = false;

    m_4DLinearRegressionGradientEstimator = 0;

    // Workaround: desactivem l'orientation marker perquè amb VTK 5.6 es renderitza cada vegada que movem el ratolí per sobre el visor.
    // TODO: buscar una solució real al problema, que no hi hagi un mouse tracking o quelcom per l'estil.
    orientationMarkerOff();
}

Q3DViewer::~Q3DViewer()
{
    /// \todo falta revisar què falta per destruir
    if (m_obscuranceMainThread && m_obscuranceMainThread->isRunning())
    {
        m_obscuranceMainThread->stop();
        m_obscuranceMainThread->wait();
        emit obscuranceCancelledByProgram();
    }
    delete m_obscuranceMainThread;
    delete m_obscurance;
    delete m_ambientVoxelShader;
    delete m_directIlluminationVoxelShader;
    delete m_contourVoxelShader;
    delete m_obscuranceVoxelShader;
    delete m_ambientContourVoxelShader;
    delete m_directIlluminationContourVoxelShader;
    delete m_ambientObscuranceVoxelShader;
    delete m_directIlluminationObscuranceVoxelShader;
    delete m_ambientContourObscuranceVoxelShader;
    delete m_directIlluminationContourObscuranceVoxelShader;

    // Eliminem tots els elements vtk creats
    if (m_4DLinearRegressionGradientEstimator)
    {
        m_4DLinearRegressionGradientEstimator->Delete();
    }
    if (m_imageData)
    {
        m_imageData->Delete();
    }
    if (m_vtkVolume)
    {
        m_vtkVolume->Delete();
    }
    if (m_volumeProperty)
    {
        m_volumeProperty->Delete();
    }
    if (m_volumeMapper)
    {
        m_volumeMapper->Delete();
    }
    m_gpuRayCastMapper->Delete();
    if (m_volumeRayCastFunction)
    {
        m_volumeRayCastFunction->Delete();
    }
    if (m_volumeRayCastAmbientContourFunction)
    {
        m_volumeRayCastAmbientContourFunction->Delete();
    }
    if (m_volumeRayCastDirectIlluminationContourFunction)
    {
        m_volumeRayCastDirectIlluminationContourFunction->Delete();
    }
    if (m_volumeRayCastAmbientObscuranceFunction)
    {
        m_volumeRayCastAmbientObscuranceFunction->Delete();
    }
    if (m_volumeRayCastDirectIlluminationObscuranceFunction)
    {
        m_volumeRayCastDirectIlluminationObscuranceFunction->Delete();
    }
    if (m_volumeRayCastAmbientContourObscuranceFunction)
    {
        m_volumeRayCastAmbientContourObscuranceFunction->Delete();
    }
    if (m_volumeRayCastDirectIlluminationContourObscuranceFunction)
    {
        m_volumeRayCastDirectIlluminationContourObscuranceFunction->Delete();
    }
    if (m_volumeRayCastIsosurfaceFunction)
    {
        m_volumeRayCastIsosurfaceFunction->Delete();
    }
    if (m_clippingPlanes)
    {
        m_clippingPlanes->Delete();
    }
}

VoiLut Q3DViewer::getCurrentVoiLut() const
{
    return m_transferFunction;
}

void Q3DViewer::setVoiLut(const VoiLut &voiLut)
{
    setTransferFunction(voiLut.getLut());
    this->applyCurrentRenderingMethod();
    emit transferFunctionChanged();
}

void Q3DViewer::setClippingPlanes(vtkPlanes *clippingPlanes)
{
    if (clippingPlanes)
    {
        m_clippingPlanes = clippingPlanes;
        m_clippingPlanes->Register(0);
        m_volumeMapper->SetClippingPlanes(m_clippingPlanes);
        m_gpuRayCastMapper->SetClippingPlanes(m_clippingPlanes);
    }
    else
    {
        DEBUG_LOG("Els plans de tall són NULS");
    }
}

vtkPlanes *Q3DViewer::getClippingPlanes() const
{
    return m_clippingPlanes;
}

void Q3DViewer::getVolumeBounds(double bounds[6]) const
{
    m_vtkVolume->GetBounds(bounds);
}

Q3DViewer* Q3DViewer::castFromQViewer(QViewer *viewer)
{
    if (!viewer)
    {
        DEBUG_LOG(QString("Cannot cast a null pointer"));
        return 0;
    }

    Q3DViewer *viewer3D = qobject_cast<Q3DViewer*>(viewer);
    if (!viewer3D)
    {
        DEBUG_LOG(QString("Q3DViewer casting failed! Provided viewer may not be a Q3DViewer object: %1").arg(viewer->metaObject()->className()));
    }

    return viewer3D;
}

void Q3DViewer::setRenderFunction(RenderFunction function)
{
    m_renderFunction = function;
}

void Q3DViewer::setRenderFunctionToRayCasting()
{
    m_renderFunction = RayCasting;
}

void Q3DViewer::setRenderFunctionToRayCastingObscurance()
{
    m_renderFunction = RayCastingObscurance;
}

void Q3DViewer::setRenderFunctionToGpuRayCasting()
{
    m_renderFunction = GpuRayCasting;
}

void Q3DViewer::setRenderFunctionToContouring()
{
    m_renderFunction = Contouring;
}

void Q3DViewer::setRenderFunctionToMIP3D()
{
    m_renderFunction = MIP3D;
}

void Q3DViewer::setRenderFunctionToIsoSurface()
{
    m_renderFunction = IsoSurface;
}

QString Q3DViewer::getRenderFunctionAsString()
{
    QString result;
    switch (m_renderFunction)
    {
        case RayCasting:
            result = "RayCasting";
            break;
        case RayCastingObscurance:
            result = "RayCastingObscurance";
            break;
        case GpuRayCasting:
            result = "GPU Ray Casting";
            break;
        case MIP3D:
            result = "MIP 3D";
            break;
        case IsoSurface:
            result = "IsoSurface";
            break;
        case Contouring:
            result = "Contouring";
            break;
    }
    return result;
}

void Q3DViewer::setInput(Volume *volume)
{
    setCursor(Qt::WaitCursor);
    // És necessari indicar que l'estat és VisualizingVolume, sinó el visor no s'actualitzarà correctament 
    // quan volguem fer canvis de window/level, zoom, moure, etc. ja que és condició necessària per dur a terme el render.
    setViewerStatus(VisualizingVolume);
    if (!checkInputVolume(volume))
    {
        unsetCursor();
        return;
    }

    if (!rescale(volume))
    {
        unsetCursor();
        return;
    }

    if (m_clippingPlanes)
    {
        m_volumeMapper->RemoveAllClippingPlanes();
        m_gpuRayCastMapper->RemoveAllClippingPlanes();
        m_clippingPlanes->Delete();
        m_clippingPlanes = 0;
    }
    m_mainVolume = volume;

    setVolumeTransformation();

    m_volumeMapper->SetInputData(m_imageData);
    m_gpuRayCastMapper->SetInputData(m_imageData);

    unsigned short *data = reinterpret_cast<unsigned short*>(m_imageData->GetPointData()->GetScalars()->GetVoidPointer(0));
    m_ambientVoxelShader->setData(data, static_cast<unsigned short>(m_range));
    m_directIlluminationVoxelShader->setData(data, static_cast<unsigned short>(m_range));

    if (m_obscuranceMainThread && m_obscuranceMainThread->isRunning())
    {
        m_obscuranceMainThread->stop();
        m_obscuranceMainThread->wait();
        emit obscuranceCancelledByProgram();
    }
    delete m_obscuranceMainThread; m_obscuranceMainThread = 0;
    delete m_obscurance; m_obscurance = 0;

    m_firstRender = true;

    int *dimensions = volume->getDimensions();

    // Ens basem només en les dimensions; de moment donem per fet que hi ha prou memòria
    m_canEnableShading = dimensions[0] > 1 && dimensions[1] > 1 && dimensions[2] > 1;

    applyCurrentRenderingMethod();

    // Indiquem el canvi de volum
    emit volumeChanged(getMainInput());

    unsetCursor();
}

void Q3DViewer::applyCurrentRenderingMethod()
{
    if (hasInput())
    {
        switch (m_renderFunction)
        {
            case Contouring:
                renderContouring();
                break;
            case RayCasting:
                renderRayCasting();
                break;
            case RayCastingObscurance:
                renderRayCastingObscurance();
                break;
            case GpuRayCasting:
                renderGpuRayCasting();
                break;
            case MIP3D:
                renderMIP3D();
                break;
            case IsoSurface:
                renderIsoSurface();
                break;
        }

        if (m_firstRender)
        {
            setDefaultViewForCurrentInput();
            m_firstRender = false;
        }
    }
    else
    {
        WARN_LOG("Q3DViewer:: Cridant a applyCurrentRenderingMethod() sense haver donat cap input");
    }
}

const TransferFunction& Q3DViewer::getTransferFunction() const
{
    return m_transferFunction;
}

void Q3DViewer::setTransferFunction(const TransferFunction &transferFunction)
{
    m_transferFunction = transferFunction;

    m_volumeProperty->SetScalarOpacity(m_transferFunction.vtkOpacityTransferFunction());
    m_volumeProperty->SetColor(m_transferFunction.vtkColorTransferFunction());
    m_ambientVoxelShader->setTransferFunction(m_transferFunction);
    m_directIlluminationVoxelShader->setTransferFunction(m_transferFunction);

    if (m_volumeProperty->GetShade())
    {
        try
        {
            vtkEncodedGradientEstimator *gradientEstimator = m_volumeMapper->GetGradientEstimator();
            m_directIlluminationVoxelShader->setEncodedNormals(gradientEstimator->GetEncodedNormals());
            vtkEncodedGradientShader *gradientShader = m_volumeMapper->GetGradientShader();
            gradientShader->UpdateShadingTable(m_renderer, m_vtkVolume, gradientEstimator);
            m_directIlluminationVoxelShader->setDiffuseShadingTables(gradientShader->GetRedDiffuseShadingTable(m_vtkVolume),
                                                                      gradientShader->GetGreenDiffuseShadingTable(m_vtkVolume),
                                                                      gradientShader->GetBlueDiffuseShadingTable(m_vtkVolume));
            m_directIlluminationVoxelShader->setSpecularShadingTables(gradientShader->GetRedSpecularShadingTable(m_vtkVolume),
                                                                       gradientShader->GetGreenSpecularShadingTable(m_vtkVolume),
                                                                       gradientShader->GetBlueSpecularShadingTable(m_vtkVolume));
        }
        catch (std::bad_alloc &e)
        {
            ERROR_LOG(QString("Excepció al voler aplicar shading en el volum: ") + e.what());
            QMessageBox::warning(this, tr("Unable to Apply Rendering Style"), tr("The system does not have enough memory to properly apply this rendering style "
                                          "with this volume.\nShading will be disabled, it will not render as expected."));
            m_canEnableShading = false;
            this->setShading(false);
            // TODO Comprovar si seria suficient amb un render()
            this->applyCurrentRenderingMethod();
        }
    }
}

void Q3DViewer::setDefaultViewForCurrentInput()
{
    // De moment, sempre serà coronal
    // TODO cal implementar que analitzi l'input i esculli la millor orientació
    resetViewToCoronal();
}

// Desplacem les dades de manera que el mínim sigui 0 i ho convertim a un unsigned short, perquè el ray casting no accepta signed short.
bool Q3DViewer::rescale(Volume *volume)
{
    if (!volume)
    {
        return false;
    }

    double range[2];
    volume->getScalarRange(range);
    
    double min = range[0];
    double max = range[1];
    double rangeLength = max - min;
    double shift = -min;
    DEBUG_LOG(QString("Q3DViewer: volume scalar range: min = %1, max = %2, range = %3, shift = %4").arg(min).arg(max).arg(rangeLength).arg(shift));

    vtkImageShiftScale *rescaler = vtkImageShiftScale::New();
    rescaler->SetInputData(volume->getVtkData());
    rescaler->SetShift(shift);
//    rescaler->SetScale(1.0);    // per defecte
    // Ho psoem en unsigned short per tal de mantenir tota la informació
    // Desavantatge: ocupa més memòria
    rescaler->SetOutputScalarTypeToUnsignedShort();
    rescaler->ClampOverflowOn();

    try
    {
        // El lloc on pot generar l'excepció és aquí
        rescaler->Update();

        if (m_imageData)
        {
            // Fem un Delete() de m_imageData perquè es destrueixi
            // La destrucció no és immediata perquè encara hi queden referències al pipeline de VTK
            m_imageData->Delete();
        }

        m_imageData = rescaler->GetOutput();
        m_imageData->Register(0);

        rescaler->Delete();

        m_range = rangeLength;

        emit scalarRange(0, m_range);

        double *newRange = m_imageData->GetScalarRange();
        DEBUG_LOG(QString("Q3DViewer: new scalar range: new min = %1, new max = %2").arg(newRange[0]).arg(newRange[1]));

        return true;
    }
    catch (std::exception &e)
    {
        ERROR_LOG(QString("Excepció al voler fer rescale(): ") + e.what());
        QMessageBox::warning(this, tr("Volume Too Large"),
                             tr("Current volume is too large. Please select another volume or close other extensions and try again."));
        // Mantenim tots els atributs de la classe tal com estaven, per rebutjar el volum netament
        rescaler->Delete();
        return false;
    }
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

    for (int row = 0; row < 3; row++)
    {
        projectionMatrix->SetElement(row, 0, currentPlaneRowVector[row]);
        projectionMatrix->SetElement(row, 1, currentPlaneColumnVector[row]);
        projectionMatrix->SetElement(row, 2, stackDirection[row]);
    }

    m_vtkVolume->SetUserMatrix(projectionMatrix);

    projectionMatrix->Delete();
}

void Q3DViewer::renderRayCasting()
{
    if (!m_renderer->HasViewProp(m_vtkVolume))
    {
        m_renderer->RemoveAllViewProps();
        m_renderer->AddViewProp(m_vtkVolume);
    }

    m_volumeProperty->DisableGradientOpacityOn();
    m_volumeProperty->SetInterpolationTypeToLinear();

    m_vtkVolume->SetMapper(m_volumeMapper);

    if (m_contourOn)
    {
        if (m_volumeProperty->GetShade())
        {
            m_volumeMapper->SetVolumeRayCastFunction(m_volumeRayCastDirectIlluminationContourFunction);
        }
        else
        {
            m_volumeMapper->SetVolumeRayCastFunction(m_volumeRayCastAmbientContourFunction);
        }
    }
    else
    {
        m_volumeMapper->SetVolumeRayCastFunction(m_volumeRayCastFunction);
    }

    if (m_contourOn && m_volumeProperty->GetShade())
    {
        vtkEncodedGradientEstimator *gradientEstimator = m_volumeMapper->GetGradientEstimator();
        m_directIlluminationVoxelShader->setEncodedNormals(gradientEstimator->GetEncodedNormals());
        vtkEncodedGradientShader *gradientShader = m_volumeMapper->GetGradientShader();
        gradientShader->UpdateShadingTable(m_renderer, m_vtkVolume, gradientEstimator);
        m_directIlluminationVoxelShader->setDiffuseShadingTables(gradientShader->GetRedDiffuseShadingTable(m_vtkVolume),
                                                                  gradientShader->GetGreenDiffuseShadingTable(m_vtkVolume),
                                                                  gradientShader->GetBlueDiffuseShadingTable(m_vtkVolume));
        m_directIlluminationVoxelShader->setSpecularShadingTables(gradientShader->GetRedSpecularShadingTable(m_vtkVolume),
                                                                   gradientShader->GetGreenSpecularShadingTable(m_vtkVolume),
                                                                   gradientShader->GetBlueSpecularShadingTable(m_vtkVolume));
    }

    setTransferFunction(m_transferFunction);

    render();
}

void Q3DViewer::renderRayCastingObscurance()
{
    if (!m_renderer->HasViewProp(m_vtkVolume))
    {
        m_renderer->RemoveAllViewProps();
        m_renderer->AddViewProp(m_vtkVolume);
    }

    m_volumeProperty->DisableGradientOpacityOn();
    m_volumeProperty->SetInterpolationTypeToLinear();

    m_vtkVolume->SetMapper(m_volumeMapper);
    if (m_obscuranceOn)
    {
        if (m_contourOn)
        {
            if (m_volumeProperty->GetShade())
            {
                m_volumeMapper->SetVolumeRayCastFunction(m_volumeRayCastDirectIlluminationContourObscuranceFunction);
            }
            else
            {
                m_volumeMapper->SetVolumeRayCastFunction(m_volumeRayCastAmbientContourObscuranceFunction);
            }
        }
        else
        {
            if (m_volumeProperty->GetShade())
            {
                m_volumeMapper->SetVolumeRayCastFunction(m_volumeRayCastDirectIlluminationObscuranceFunction);
            }
            else
            {
                m_volumeMapper->SetVolumeRayCastFunction(m_volumeRayCastAmbientObscuranceFunction);
            }
        }
    }
    else if (m_contourOn)
    {
        if (m_volumeProperty->GetShade())
        {
            m_volumeMapper->SetVolumeRayCastFunction(m_volumeRayCastDirectIlluminationContourFunction);
        }
        else
        {
            m_volumeMapper->SetVolumeRayCastFunction(m_volumeRayCastAmbientContourFunction);
        }
    }
    else
    {
        m_volumeMapper->SetVolumeRayCastFunction(m_volumeRayCastFunction);
    }

    if ((m_contourOn || m_obscuranceOn) && m_volumeProperty->GetShade())
    {
        vtkEncodedGradientEstimator *gradientEstimator = m_volumeMapper->GetGradientEstimator();
        m_directIlluminationVoxelShader->setEncodedNormals(gradientEstimator->GetEncodedNormals());
        vtkEncodedGradientShader *gradientShader = m_volumeMapper->GetGradientShader();
        gradientShader->UpdateShadingTable(m_renderer, m_vtkVolume, gradientEstimator);
        m_directIlluminationVoxelShader->setDiffuseShadingTables(gradientShader->GetRedDiffuseShadingTable(m_vtkVolume),
                                                                  gradientShader->GetGreenDiffuseShadingTable(m_vtkVolume),
                                                                  gradientShader->GetBlueDiffuseShadingTable(m_vtkVolume));
        m_directIlluminationVoxelShader->setSpecularShadingTables(gradientShader->GetRedSpecularShadingTable(m_vtkVolume),
                                                                   gradientShader->GetGreenSpecularShadingTable(m_vtkVolume),
                                                                   gradientShader->GetBlueSpecularShadingTable(m_vtkVolume));
    }

    setTransferFunction(m_transferFunction);

    render();
}

void Q3DViewer::renderGpuRayCasting()
{
    if (!m_renderer->HasViewProp(m_vtkVolume))
    {
        m_renderer->RemoveAllViewProps();
        m_renderer->AddViewProp(m_vtkVolume);
    }

    m_volumeProperty->DisableGradientOpacityOn();
    m_volumeProperty->SetInterpolationTypeToLinear();

    m_vtkVolume->SetMapper(m_gpuRayCastMapper);

    // TODO Realment cal tornar a assignar la funció de transferència?
    setTransferFunction(m_transferFunction);

    render();
}

void Q3DViewer::renderMIP3D()
{
    if (!m_renderer->HasViewProp(m_vtkVolume))
    {
        m_renderer->RemoveAllViewProps();
        m_renderer->AddViewProp(m_vtkVolume);
    }

    // Quan fem MIP3D deixarem disable per defecte ja que la orientació no la sabem ben bé quina és ja que el pla de tall pot ser arbitrari \TODO no sempre un
    // mip serà sobre un pla mpr, llavors tampoc és del tot correcte decidir això aquí
//         m_orientationMarker->disable();
    // ================================================================================================
    // Create a transfer function mapping scalar value to opacity
    // assignem una rampa d'opacitat total per valors alts i nula per valors petits
    // després en l'escala de grisos donem un  valor de gris constant (blanc)

    // \TODO Les funcions de transferència no es definiran "a pelo" aquí mai més. Això és cosa de la classe TransferFunction

    // Creem la funció de transferència de l'opacitat
//     m_transferFunction->addPointToOpacity(20, .0);
//     m_transferFunction->addPointToOpacity(255, 1.);
    TransferFunction mipTransferFunction;
    mipTransferFunction.setOpacity(20.0, 0.0);
    mipTransferFunction.setOpacity(m_range, 1.0);

    // Creem la funció de transferència de colors
    // Create a transfer function mapping scalar value to color (grey)
    vtkPiecewiseFunction *grayTransferFunction = vtkPiecewiseFunction::New();
    grayTransferFunction->AddSegment(0, 0.0, m_range, 1.0);

//     m_volumeProperty->SetScalarOpacity(m_transferFunction->getOpacityTransferFunction());
    m_volumeProperty->SetScalarOpacity(mipTransferFunction.vtkOpacityTransferFunction());
    m_volumeProperty->SetColor(grayTransferFunction /*m_transferFunction->vtkColorTransferFunction()*/);
    m_volumeProperty->SetInterpolationTypeToLinear();

    grayTransferFunction->Delete();

    // Creem la funció del raig MIP, en aquest cas maximitzem l'opacitat, si fos Scalar value, ho faria pel valor
    vtkVolumeRayCastMIPFunction *mipFunction = vtkVolumeRayCastMIPFunction::New();
    mipFunction->SetMaximizeMethodToOpacity();

//         vtkFiniteDifferenceGradientEstimator *gradientEstimator = vtkFiniteDifferenceGradientEstimator::New();
//     vtkVolumeRayCastMapper* volumeMapper = vtkVolumeRayCastMapper::New();

    m_vtkVolume->SetMapper(m_volumeMapper);
    m_volumeMapper->SetVolumeRayCastFunction(mipFunction);
//     volumeMapper->SetInputConnection(m_imageCaster->GetOutputPort() );
//         volumeMapper->SetGradientEstimator(gradientEstimator);

//     m_vtkVolume->SetMapper(volumeMapper);

    mipFunction->Delete();

    render();
}

void Q3DViewer::renderContouring()
{
    if (m_renderer->HasViewProp(m_vtkVolume))
    {
        vtkImageShrink3D *shrink = vtkImageShrink3D::New();
        shrink->SetInputData(getMainInput()->getVtkData());
        vtkImageGaussianSmooth *smooth = vtkImageGaussianSmooth::New();
        smooth->SetDimensionality(3);
        smooth->SetRadiusFactor(2);
        smooth->SetInputConnection(shrink->GetOutputPort());

        vtkContourFilter *contour = vtkContourFilter::New();
        contour->SetInputConnection(smooth->GetOutputPort());
        contour->GenerateValues(1, 30, 30);
        contour->ComputeScalarsOff();
        contour->ComputeGradientsOff();

        vtkDecimatePro *decimator = vtkDecimatePro::New();
        decimator->SetInputConnection(contour->GetOutputPort());
        decimator->SetTargetReduction(0.9);
        decimator->PreserveTopologyOn();

        vtkReverseSense *reverse = vtkReverseSense::New();
        reverse->SetInputConnection(decimator->GetOutputPort());
        reverse->ReverseCellsOn();
        reverse->ReverseNormalsOn();

        vtkPolyDataMapper *polyDataMapper = vtkPolyDataMapper::New();

        polyDataMapper->SetInputConnection(reverse->GetOutputPort());
        polyDataMapper->ScalarVisibilityOn();
        polyDataMapper->ImmediateModeRenderingOn();

        vtkActor *actor = vtkActor::New();
        actor->SetMapper(polyDataMapper);
        actor->GetProperty()->SetColor(1, 0.8, 0.81);

        m_renderer->RemoveViewProp(m_vtkVolume);
        m_renderer->AddViewProp(actor);

        decimator->Delete();
        actor->Delete();
        polyDataMapper->Delete();
        contour->Delete();
        smooth->Delete();
        shrink->Delete();
        reverse->Delete();
    }

    render();
}

void Q3DViewer::renderIsoSurface()
{
    if (!m_renderer->HasViewProp(m_vtkVolume))
    {
        m_renderer->RemoveAllViewProps();
        m_renderer->AddViewProp(m_vtkVolume);
    }

    // \TODO Les funcions de transferència no es definiran "a pelo" aquí mai més. Això és cosa de la classe TransferFunction
    // Create a transfer function mapping scalar value to opacity
    vtkPiecewiseFunction *oTFun = vtkPiecewiseFunction::New();
    oTFun->AddSegment(0.0, 0.0, m_range, 0.3);

//     vtkPiecewiseFunction *opacityTransferFunction = vtkPiecewiseFunction::New();
//     opacityTransferFunction->AddSegment( 0, 0.0, 128, 1.0);
//     opacityTransferFunction->AddSegment(128, 1.0, 255, 0.0);

    // Create a transfer function mapping scalar value to color (grey)
//     vtkPiecewiseFunction *grayTransferFunction = vtkPiecewiseFunction::New();
//     grayTransferFunction->AddSegment(0, 1.0, 255, 1.0);

    // Create a transfer function mapping scalar value to color (color)
    vtkColorTransferFunction *cTFun = vtkColorTransferFunction::New();
    cTFun->AddRGBPoint(0.0, 1.0, 0.0, 0.0);
    cTFun->AddRGBPoint(0.25 * m_range, 1.0, 1.0, 0.0);
    cTFun->AddRGBPoint(0.50 * m_range, 0.0, 1.0, 0.0);
    cTFun->AddRGBPoint(0.75 * m_range, 0.0, 1.0, 1.0);
    cTFun->AddRGBPoint(m_range, 0.0, 0.0, 1.0);

    // Create a transfer function mapping magnitude of gradient to opacity
    vtkPiecewiseFunction *goTFun = vtkPiecewiseFunction::New();
    goTFun->AddPoint(0, 0.0);
    goTFun->AddPoint(30, 0.0);
    goTFun->AddPoint(40, 1.0);
    goTFun->AddPoint(255, 1.0);

    m_volumeProperty->ShadeOn();

    m_volumeProperty->SetScalarOpacity(oTFun);
    m_volumeProperty->DisableGradientOpacityOff();
    m_volumeProperty->SetGradientOpacity(goTFun);
    m_volumeProperty->SetColor(cTFun);
//     m_volumeProperty->SetColor(grayTransferFunction);
    m_volumeProperty->SetInterpolationTypeToLinear(); //prop[index]->SetInterpolationTypeToNearest();

    m_vtkVolume->SetMapper(m_volumeMapper);
    m_volumeMapper->SetVolumeRayCastFunction(m_volumeRayCastIsosurfaceFunction);

    oTFun->Delete();
    cTFun->Delete();
    goTFun->Delete();

    render();
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

void Q3DViewer::computeObscurance(ObscuranceQuality quality)
{
    Q_ASSERT(!m_obscuranceMainThread || m_obscuranceMainThread->isFinished());

    delete m_obscuranceMainThread; m_obscuranceMainThread = 0;
    delete m_obscurance;
    m_obscurance = 0;

    if (!m_4DLinearRegressionGradientEstimator)
    {
        m_4DLinearRegressionGradientEstimator = Vtk4DLinearRegressionGradientEstimator::New();
        // Radi 1 per defecte (-> 3³)
        m_volumeMapper->SetGradientEstimator(m_4DLinearRegressionGradientEstimator);
        /// \TODO hauria de funcionar sense això, però no !?!?!
        m_4DLinearRegressionGradientEstimator->SetInputData(m_volumeMapper->GetInput());
    }

    Settings settings;
    int numberOfDirections;
    ObscuranceMainThread::Function function;
    ObscuranceMainThread::Variant variant;
    unsigned int gradientRadius;
    switch (quality)
    {
        case Low:
            numberOfDirections = settings.getValue(CoreSettings::NumberOfDirectionsForLowQualityObscurances).toInt();
            function = static_cast<ObscuranceMainThread::Function>(settings.getValue(CoreSettings::FunctionForLowQualityObscurances).toInt());
            variant = static_cast<ObscuranceMainThread::Variant>(settings.getValue(CoreSettings::VariantForLowQualityObscurances).toInt());
            gradientRadius = settings.getValue(CoreSettings::GradientRadiusForLowQualityObscurances).toUInt();
            break;

        case Medium:
            numberOfDirections = settings.getValue(CoreSettings::NumberOfDirectionsForMediumQualityObscurances).toInt();
            function = static_cast<ObscuranceMainThread::Function>(settings.getValue(CoreSettings::FunctionForMediumQualityObscurances).toInt());
            variant = static_cast<ObscuranceMainThread::Variant>(settings.getValue(CoreSettings::VariantForMediumQualityObscurances).toInt());
            gradientRadius = settings.getValue(CoreSettings::GradientRadiusForMediumQualityObscurances).toUInt();
            break;

        case High:
            numberOfDirections = settings.getValue(CoreSettings::NumberOfDirectionsForHighQualityObscurances).toInt();
            function = static_cast<ObscuranceMainThread::Function>(settings.getValue(CoreSettings::FunctionForHighQualityObscurances).toInt());
            variant = static_cast<ObscuranceMainThread::Variant>(settings.getValue(CoreSettings::VariantForHighQualityObscurances).toInt());
            gradientRadius = settings.getValue(CoreSettings::GradientRadiusForHighQualityObscurances).toUInt();
            break;

        default:
            ERROR_LOG(QString("Valor inesperat per a la qualitat de les obscurances: %1").arg(quality));
    }

    /// \TODO de moment la meitat de la diagonal, però podria ser una altra funció
    double distance = m_vtkVolume->GetLength() / 2.0;
    // El primer paràmetre és el nombre de direccions
    // pot ser >= 0 i llavors es fan 10*4^n+2 direccions (12, 42, 162, 642, ...)
    // també pot ser < 0 i llavors es fan -n direccions (valors permesos: -4, -6, -8, -12, -20; amb qualsevol altre s'aplica -4)
    m_obscuranceMainThread = new ObscuranceMainThread(numberOfDirections, distance, function, variant, this);

    /// \todo Només canviant això ja recalcularà les normals o cal fer alguna cosa més?
    if (m_4DLinearRegressionGradientEstimator->getRadius() < gradientRadius)
    {
        m_4DLinearRegressionGradientEstimator->setRadius(gradientRadius);
    }

    m_obscuranceMainThread->setVolume(m_vtkVolume);
    m_obscuranceMainThread->setTransferFunction(m_transferFunction);

    connect(m_obscuranceMainThread, SIGNAL(progress(int)), this, SIGNAL(obscuranceProgress(int)));
    connect(m_obscuranceMainThread, SIGNAL(computed()), this, SLOT(endComputeObscurance()));
    m_obscuranceMainThread->start();

    // Perquè el DirectIlluminationVoxelShader tingui les noves normals
    setTransferFunction(m_transferFunction);

    // Perquè el ContourVoxelShader tingui les noves normals
    setContour(m_contourOn);
}

void Q3DViewer::cancelObscurance()
{
    Q_ASSERT(m_obscuranceMainThread && m_obscuranceMainThread->isRunning());

    m_obscuranceMainThread->stop();
}

void Q3DViewer::endComputeObscurance()
{
    Q_ASSERT(m_obscuranceMainThread);

    m_obscurance = m_obscuranceMainThread->getObscurance();
    m_obscuranceVoxelShader->setObscurance(m_obscurance);

    emit obscuranceComputed();
}

void Q3DViewer::setObscurance(bool on)
{
    m_obscuranceOn = on;
}

void Q3DViewer::setObscuranceFactor(double factor)
{
    m_obscuranceVoxelShader->setFactor(factor);
}

void Q3DViewer::setContour(bool on)
{
    m_contourOn = on;

    if (on)
    {
        m_contourVoxelShader->setGradientEstimator(m_volumeMapper->GetGradientEstimator());
    }
}

void Q3DViewer::setContourThreshold(double threshold)
{
    m_contourVoxelShader->setThreshold(threshold);
}

void Q3DViewer::setIsoValue(int isoValue)
{
    m_volumeRayCastIsosurfaceFunction->SetIsoValue(isoValue);
}

void Q3DViewer::getCurrentRenderedItemBounds(double bounds[6])
{
    m_vtkVolume->GetBounds(bounds);
}

bool Q3DViewer::checkInputVolume(Volume *volume)
{
    if (!volume)
    {
        DEBUG_LOG("El volum és NUL");
        WARN_LOG("El volum és NUL");
        return false;
    }

    // Comprovem si tenim imatges
    if (volume->getImages().isEmpty())
    {
        DEBUG_LOG("El volum no conté imatges");
        WARN_LOG("El volum no conté imatges");
        return false;
    }

    if (!isSupportedVolume(volume))
    {
        DEBUG_LOG("El format del volum no està suportat");
        WARN_LOG("El format del volum no està suportat.");
        QMessageBox::warning(this, tr("Volume Not Supported"), tr("Current volume cannot be opened because its format is not supported."));
        return false;
    }

    return true;
}

bool Q3DViewer::canAllocateMemory(int size)
{
    char *p = 0;
    try
    {
        p = new char[size];
        delete[] p;
        return true;
    }
    catch (std::bad_alloc)
    {
        return false;
    }
}

bool Q3DViewer::isSupportedVolume(Volume *volume)
{
    double range[2];
    volume->getScalarRange(range);
    return volume->getNumberOfScalarComponents() == 1 && range[1] > range[0];
}

};  // End namespace udg {
