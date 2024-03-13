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

#include <QSplitter>
#include <QVBoxLayout>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QFileDialog>
#include <QDebug>

#include <vtkRenderer.h>
#include <vtkImageReslice.h>
#include <vtkSmartVolumeMapper.h>
#include <vtkVolumeProperty.h>
#include <vtkPiecewiseFunction.h>
#include <vtkColorTransferFunction.h>
#include <vtkLookupTable.h>
#include <vtkCamera.h>
#include <vtkMatrix4x4.h>

#include "qdlsegmentationextension.h"
#include "deeplearningsegmentation.h"
#include "deeplearningtensorflowsegmentation.h"
#include "deeplearningtorchsegmentation.h"
#include "patient.h"
#include "toolmanager.h"
#include "volume.h"
#include "drawer.h"
#include "drawerpolygon.h"
#include "drawertext.h"

// Adding a Q_DECLARE_METATYPE() makes the type known to all template based
// functions, including QVariant.
// Ideally, this macro should be placed below the declaration of the class or
// struct. If that is not possible, it can be put in a private header file
// which has to be included every time that type is used in a QVariant.
// #include <QMetaType> may be needed.
// If placed below the declaration, QMetaType must be included, and there is
// conflict with `slots` definition when including libtorch library.
// Workaround:
// #undef slots
// #include <torch/script.h>
// #define slots Q_SLOTS
Q_DECLARE_METATYPE(udg::DeepLearningSegmentation::ActivationFunction);

namespace udg {

struct QDLSegmentationExtension::ModelParameters {
    QString path;
    QVector<int> inputDimensions;
    bool inputNormalised;
    int inputChannels;
    int outputLabels;
    udg::DeepLearningSegmentation::ActivationFunction outputActivationFunction;
};

QDLSegmentationExtension::QDLSegmentationExtension(QWidget *parent)
    : QWidget(parent)
{
    setupUi(this);

    // Make viewers take up the main part of the window
    m_inputSplitter->setStretchFactor(0, 1);
    m_inputSplitter->setStretchFactor(1, 5);

    // Set the same width to each viewer (only relative size matters)
    int width = std::max(m_2DViewerVLayout->minimumSize().width(), m_3DViewerVLayout->minimumSize().width());
    m_viewersSplitter->setSizes({width, width});

    // Add predefined trained models

    // m_predefinedTrainedModels.append({"Name", {
    //     "path/to/model",
    //     {inputDimX, inputDimY, inputDimZ},
    //     inputNormalised,
    //     inputChannels,
    //     outputLabels,
    //     outputActivationFunction
    // }});

    m_predefinedTrainedModels.append({"Custom", {}}); // must be the last one

    for (const QPair<QString, ModelParameters>& model : m_predefinedTrainedModels)
    {
        m_trainedModelPredefinedCombo->addItem(model.first);
    }

    // Set "custom" option in italics
    QFont customFont = m_trainedModelPredefinedCombo->font();
    customFont.setItalic(true);
    m_trainedModelPredefinedCombo->setItemData(m_trainedModelPredefinedCombo->count()-1, customFont, Qt::FontRole);

    if (m_predefinedTrainedModels.size() > 1) {
        m_trainedModelCustomWidget->hide();
    }

    // Add activation functions
    m_outputParamFunctionCombo->addItem("Sigmoid", DeepLearningSegmentation::ActivationFunction::SIGMOID);
    m_outputParamFunctionCombo->addItem("Softmax", DeepLearningSegmentation::ActivationFunction::SOFTMAX);

    // Set interface parameters according to default predefined model
    predefinedTrainedModelChanged(m_trainedModelPredefinedCombo->currentIndex());

    m_croppingArea = nullptr;
    m_croppingAreaSlice = -1; // no cropping area at the beginning
    m_sliceRange = {0, 0}; // only first by default
    m_2DMask = nullptr;
    m_3DMask = nullptr;
    m_DLSegmentation = nullptr;
    m_maskData = nullptr;

    createConnections();
    initializeTools();
}

void QDLSegmentationExtension::createConnections()
{
    // Remove the cropping area (if any) if the anatomical plane is changed
    connect(m_2DViewer->getViewer(), &Q2DViewer::anatomicalViewChanged, this, &QDLSegmentationExtension::removeCroppingArea);

    // Restore 3D viewer if restore button is clicked (removes 3D mask)
    // (should be an action tool, but here the generic behaviour of the 3D
    // viewer restore tool is reimplemented, so a new function is needed)
    connect(m_restore3DToolButton, &QToolButton::clicked, this, &QDLSegmentationExtension::restore3DViewer);

    // Assign black&white colour function each time the volume is changed
    // In lambda, class member `m_3DViewer` is captured by reference and called `viewer`
    connect(m_3DViewer, &Q3DViewer::volumeChanged, [&viewer = m_3DViewer](Volume *volume) {
        double range[2];
        volume->getScalarRange(range);
        TransferFunction clut;
        clut.set(range[0], Qt::black, 0.0);
        clut.set(range[1], Qt::white, 1.0);
        viewer->setTransferFunction(clut);
    });

    // Update the cropping area each time a new ROI is added to or removed from
    // the viewer
    connect(m_2DViewer->getViewer()->getDrawer(), &Drawer::primitiveUpdated, this, &QDLSegmentationExtension::primitiveUpdated);

    // Change drawing tool button text (reset each time is toggled)
    // In lambda, class member `m_drawingToolButton` is captured by reference
    // and called `tool`
    connect(m_drawingToolButton, &QPushButton::toggled, [&tool = m_drawingToolButton]() {
        tool->setText(tr("Crop"));
    });

    // Assign interface button actions
    connect(m_trainedModelPredefinedCombo, qOverload<int>(&QComboBox::currentIndexChanged), this, &QDLSegmentationExtension::predefinedTrainedModelChanged);
    connect(m_trainedModelCustomBrowseButton, &QPushButton::clicked, this, &QDLSegmentationExtension::browseCustomTrainedModel);
    connect(m_applyPushButton, &QPushButton::clicked, this, &QDLSegmentationExtension::apply);
}

void QDLSegmentationExtension::initializeTools()
{
    // Create the 2D tool manager
    ToolManager *toolManager2D = new ToolManager(this);

    // Register actions and bind them to interface buttons
    m_slicingToolButton->setDefaultAction(toolManager2D->registerTool("SlicingMouseTool")); // left button
    m_zoomToolButton->setDefaultAction(toolManager2D->registerTool("ZoomTool"));            // left button
    m_drawingToolButton->setDefaultAction(toolManager2D->registerTool("PolylineROITool"));  // left button
    m_eraserToolButton->setDefaultAction(toolManager2D->registerTool("EraserTool"));        // left button
    m_restore2DToolButton->setDefaultAction(toolManager2D->registerActionTool("RestoreActionTool"));
    m_voxelInformationToolButton->setDefaultAction(toolManager2D->registerTool("VoxelInformationTool")); // voxel information
    m_axialViewToolButton->setDefaultAction(toolManager2D->registerActionTool("AxialViewActionTool"));
    m_sagittalViewToolButton->setDefaultAction(toolManager2D->registerActionTool("SagitalViewActionTool"));
    m_coronalViewToolButton->setDefaultAction(toolManager2D->registerActionTool("CoronalViewActionTool"));
    m_rotateClockwiseToolButton->setDefaultAction(toolManager2D->registerActionTool("RotateClockWiseActionTool"));
    m_rotateCounterClockwiseToolButton->setDefaultAction(toolManager2D->registerActionTool("RotateCounterClockWiseActionTool"));
    m_flipHorizontalToolButton->setDefaultAction(toolManager2D->registerActionTool("HorizontalFlipActionTool"));
    m_flipVerticalToolButton->setDefaultAction(toolManager2D->registerActionTool("VerticalFlipActionTool"));

    // Register actions not bound to interface buttons
    toolManager2D->registerTool("ZoomTool");            // left button
    toolManager2D->registerTool("TranslateTool");       // middle button
    toolManager2D->registerTool("WindowLevelTool");     // right button
    toolManager2D->registerTool("SlicingKeyboardTool"); // keyboard
    toolManager2D->registerTool("SlicingWheelTool");    // wheel


    // Create the 3D tool manager
    ToolManager *toolManager3D = new ToolManager(this);

    // Register actions and bind them to interface buttons
    m_rotate3DToolButton->setDefaultAction(toolManager3D->registerTool("Rotate3DTool"));       // right button
    m_windowLevelToolButton->setDefaultAction(toolManager3D->registerTool("WindowLevelTool")); // right button
//    m_restore3DToolButton->setDefaultAction(toolManager3D->registerActionTool("RestoreActionTool"));

    // Register actions not bound to interface buttons
    toolManager3D->registerTool("ZoomTool");        // left button
    toolManager3D->registerTool("ZoomWheelTool");   // mouse wheel
    toolManager3D->registerTool("TranslateTool");   // middle button
    toolManager3D->registerTool("Rotate3DTool");    // right button


    // Enable default tools (same as if triggered each one individually)
    toolManager2D->triggerTools({"SlicingMouseTool", "WindowLevelTool", "TranslateTool", "SlicingKeyboardTool", "SlicingWheelTool"});
    toolManager3D->triggerTools({"ZoomTool", "ZoomWheelTool", "TranslateTool", "Rotate3DTool"});

    // Define exclusive groups
    // (e.g. right button for rotating or WW/WL, not for both)
    QStringList leftButtonExclusiveTools;
    leftButtonExclusiveTools << "SlicingMouseTool" << "ZoomTool" << "PolylineROITool" << "EraserTool";
    toolManager2D->addExclusiveToolsGroup("LeftButtonGroup", leftButtonExclusiveTools);

    QStringList rightButtonExclusiveTools;
    rightButtonExclusiveTools << "WindowLevelTool" << "Rotate3DTool";
    toolManager3D->addExclusiveToolsGroup("RightButtonGroup", rightButtonExclusiveTools);

    // Initialise tools and bind them to the corresponding viewer
    toolManager2D->setupRegisteredTools(m_2DViewer->getViewer());
    toolManager2D->enableRegisteredActionTools(m_2DViewer->getViewer());
    toolManager3D->setupRegisteredTools(m_3DViewer);

    // Change drawing tool button text
    m_drawingToolButton->setText(tr("Crop"));
}

void QDLSegmentationExtension::setPatient(Patient *patient)
{
    if (patient->getNumberOfVolumes() == 0)
    {
        return;
    }

    // Get patient volume (main volume) and initialize viewers
    Volume* patientVolume = patient->getVolumesList().constFirst();

    // Create mask (same properties as patient volume) and allocate scalars
    vtkImageData* imageData = patientVolume->getVtkData();
    m_maskData = vtkSmartPointer<vtkImageData>::New();
    m_maskData->SetDimensions(imageData->GetDimensions());
    m_maskData->SetExtent(imageData->GetExtent());
    m_maskData->SetSpacing(imageData->GetSpacing());
    m_maskData->SetOrigin(imageData->GetOrigin());
    m_maskData->AllocateScalars(VTK_SHORT, 1);

    // Initialise viewers with the patient volume and the mask
    initialize2DViewer(patientVolume);
    initialize3DViewer(patientVolume);

    // Add a transfer function to both viewers for the mask
    createTransferFunction();
}

void QDLSegmentationExtension::browseCustomTrainedModel()
{
    // Open file dialog to select the trained model path
    QString path = QFileDialog::getExistingDirectory(this, tr("Select trained model directory"),
                                                     "",
                                                     QFileDialog::ShowDirsOnly |
                                                     QFileDialog::DontResolveSymlinks |
                                                     QFileDialog::DontUseNativeDialog);

    if (!path.isEmpty()) {
        m_trainedModelCustomLineEdit->setText(path);
    }
}

void QDLSegmentationExtension::predefinedTrainedModelChanged(int index)
{
    // If "custom" is selected, show text box to enter the trained model path;
    // otherwise, hide it and set the corresponding interface parameters
    // according to the predefined trained model selected
    if (index == m_trainedModelPredefinedCombo->count()-1)
    {
        m_trainedModelCustomWidget->show();
    }
    else
    {
        m_inputParamDimXSpinBox->setValue(m_predefinedTrainedModels.at(index).second.inputDimensions[0]);
        m_inputParamDimYSpinBox->setValue(m_predefinedTrainedModels.at(index).second.inputDimensions[1]);
        m_inputParamDimZSpinBox->setValue(m_predefinedTrainedModels.at(index).second.inputDimensions[2]);
        m_inputParamNormalisationCheckBox->setChecked(m_predefinedTrainedModels.at(index).second.inputNormalised);
        m_inputParamChannelsSpinBox->setValue(m_predefinedTrainedModels.at(index).second.inputChannels);
        m_outputParamLabelsSpinBox->setValue(m_predefinedTrainedModels.at(index).second.outputLabels);
        int comboId = m_outputParamFunctionCombo->findData(m_predefinedTrainedModels.at(index).second.outputActivationFunction);
        m_outputParamFunctionCombo->setCurrentIndex(comboId);

        m_trainedModelCustomWidget->hide();
    }
}

void QDLSegmentationExtension::primitiveUpdated(DrawerPrimitive *primitive, bool add)
{
    // Cast primitive to check subclass:
    // - Polygon: ROI related to cropping area
    // - Text: annotations to get rid of

    if (DrawerPolygon* polygon = dynamic_cast<DrawerPolygon*>(primitive)) {
        // Check whether polygon has to be added and drawing tool is selected
        if (add && m_drawingToolButton->isChecked())
        {
            // Remove previous cropping area (if any) from 2D viewer
            removeCroppingArea();

            // Update the cropping area
            m_croppingArea = polygon;
            m_croppingAreaSlice = m_2DViewer->getViewer()->getCurrentSlice();
        }
        else if (!add && m_croppingArea == polygon)
        {
            // If the polygon is removed and is the one stored, set cropping
            // area to null
            m_croppingArea = nullptr;
        }
    }
    else if (DrawerText* text = dynamic_cast<DrawerText*>(primitive)) {
        // If 2D viewer is adding an annotation, remove it
        // NOTE: be careful, "add" check needed to avoid endless loop by
        // erasing the same primitive
        if (add) {
            m_2DViewer->getViewer()->getDrawer()->erasePrimitive(primitive);
            m_2DViewer->getViewer()->render();
        }
    }
}

void QDLSegmentationExtension::restore3DViewer()
{
    // Hide 3D mask
    m_3DMask->SetVisibility(false);

    // Reset view to initial state
    m_3DViewer->resetViewToCoronal();

    m_3DViewer->render();
}

void QDLSegmentationExtension::removeCroppingArea()
{
    // Remove previous cropping area (if any) from 2D viewer
    if (m_croppingArea) {
        m_2DViewer->getViewer()->getDrawer()->erasePrimitive(m_croppingArea);
        m_croppingArea = nullptr;
    }
}

void QDLSegmentationExtension::getCroppingBounds(QVector<int>& bounds) const
{
    // Get input image data
    vtkImageData* imageData = m_3DViewer->getMainInput()->getVtkData();

    // Initialise bounds to full image extent
    imageData->GetExtent(bounds.data());

    if (m_croppingArea) {
        // Find bounding box surrounding the cropping area, regardless of the
        // order of the points
        double worldBounds[6];
        m_croppingArea->getBounds(worldBounds);
        double minPoint[3] = {worldBounds[0], worldBounds[2], worldBounds[4]};
        double maxPoint[3] = {worldBounds[1], worldBounds[3], worldBounds[5]};

        // Get voxel coordinates of the minimum and maximum 3D point
        int ijkMin[3], ijkMax[3];
        double pcoords[3];
        imageData->ComputeStructuredCoordinates(minPoint, ijkMin, pcoords);
        imageData->ComputeStructuredCoordinates(maxPoint, ijkMax, pcoords);

        // Assign bounds for each coordinate (ensuring it is in bounds with
        // the main volume)
        for (int i = 0; i < 3; i++) {
            if (ijkMin[i] < bounds[2*i]) {
                ijkMin[i] = bounds[2*i];
            }
            else if (ijkMin[i] > bounds[2*i + 1]) {
                ijkMin[i] = bounds[2*i + 1];
            }
            bounds[2*i] = ijkMin[i];

            if (ijkMax[i] < bounds[2*i]) {
                ijkMax[i] = bounds[2*i];
            }
            else if (ijkMax[i] > bounds[2*i + 1]) {
                ijkMax[i] = bounds[2*i + 1];
            }
            bounds[2*i + 1] = ijkMax[i];
        }
    }

    // Get current view plane
    OrthogonalPlane orthogonalPlane = m_2DViewer->getViewer()->getView();

    // The Z axis will differ depending on current view plane
    int zAxis;
    switch (orthogonalPlane)
    {
        case OrthogonalPlane::YZPlane:
            zAxis = 0;
            break;

        case OrthogonalPlane::XZPlane:
            zAxis = 1;
            break;

        case OrthogonalPlane::XYPlane:
        default:
            zAxis = 2;
            break;
    }

    // Set the slice range to the current Z axis bounds
    bounds[2 * zAxis] = m_sliceRange[0];
    bounds[2 * zAxis + 1] = m_sliceRange[1];
}

void QDLSegmentationExtension::renderMask2D()
{
    // Clear drawing primitives from the 2D viewer and render 2D mask
    m_2DViewer->getViewer()->clearViewer();
    m_2DViewer->getViewer()->render();
}

void QDLSegmentationExtension::createTransferFunction()
{
    // Get number of labels
    int numLabels = m_outputParamLabelsSpinBox->value();

    // Create a new LUT where 0 is transparent
    vtkNew<vtkLookupTable> lut;
    lut->SetNumberOfTableValues(numLabels);
    lut->SetTableRange(0, numLabels - 1);
    lut->SetTableValue(0, 0.0, 0.0, 0.0, 0.0); // 0: black, transparent

    // Create a new transfer function where 0 is black
    vtkNew<vtkColorTransferFunction> color;
    color->AddRGBPoint(0, 0.0, 0.0, 0.0); // 0: black

    // Assign a colour for each label (equally distributed along hue range)
    for (int label = 1; label < numLabels; label++) {
        // Compute HSV for current label (S = V = 1, i.e. max value)
        double hue = (1.0 / (numLabels - 1)) * (label - 1);
        double hsv[3] = {hue, 1.0, 1.0};

        // Convert HSV to RGB values
        double rgb[3];
        vtkMath::HSVToRGB(hsv, rgb);

        // Set colour to LUT and transfer function
        lut->SetTableValue(label, rgb[0], rgb[1], rgb[2], 1.0); // 1.0 = opaque
        color->AddRGBPoint(label, rgb[0], rgb[1], rgb[2]);
    }

    // Build LUT
    lut->Build();

    // Set transfer function to 2D mask volume (index 1)
    TransferFunction transferFunction(lut);
    m_2DViewer->getViewer()->setVolumeTransferFunction(1, transferFunction);

    // Set fixed WW/WL to 2D mask volume (index 1), so that values > 0 are
    // displayed (width = label range, level = half label range)
    // 0: black / numLabels-1: white
    WindowLevel windowLevel(numLabels - 1, (numLabels - 1) / 2.0);
    VoiLut voiLut(windowLevel);
    m_2DViewer->getViewer()->setVoiLutInVolume(1, voiLut);

    // Set new transfer function for 3D mask volume
    m_3DMask->GetProperty()->SetColor(color);
}

void QDLSegmentationExtension::renderMask3D()
{
    // Render 3D mask volume
    m_3DMask->SetVisibility(true);
    m_3DViewer->render();
}

void QDLSegmentationExtension::initialize2DViewer(Volume *mainVolume)
{
    // Create a volume for the 2D mask
    m_2DMask = new Volume(); // QPointer automatically deleted

    // Set image properties from main volume to mask volume
    m_2DMask->setImages(mainVolume->getImages());

    // Set mask data (voxel data)
    m_2DMask->setData(m_maskData);

    // Set two input volumes to the 2D viewer (fusion: mask can be faded)
    QList<Volume*> volumeList = {mainVolume, m_2DMask};
    m_2DViewer->setInputAsynchronously(volumeList);
}

void QDLSegmentationExtension::initialize3DViewer(Volume *mainVolume)
{
    // Set the main volume of the 3D viewer
    m_3DViewer->setInput(mainVolume);


    // **
    // * Prepare a new volume for the 3D mask
    // **

    // Create a new mapper with the default input mask
    vtkNew<vtkSmartVolumeMapper> volumeMapper;
    volumeMapper->SetBlendModeToComposite();
    volumeMapper->SetInputData(m_maskData);

    // Create a new property for the volume rendering
    vtkNew<vtkVolumeProperty> volumeProperty;
    volumeProperty->ShadeOff();
    volumeProperty->SetInterpolationTypeToNearest(); // not linear for mask

    // Set opacity to 0 for non-mask voxels
    vtkNew<vtkPiecewiseFunction> compositeOpacity;
    compositeOpacity->AddPoint(0.0, 0.0);
    compositeOpacity->AddPoint(1.0, 1.0); // values >= 1 are all opaque
    volumeProperty->SetScalarOpacity(compositeOpacity);

    // Create the new volume and assign current volume's orientation
    m_3DMask = vtkSmartPointer<vtkVolume>::New();
    m_3DMask->SetMapper(volumeMapper);
    m_3DMask->SetProperty(volumeProperty);
    m_3DMask->SetUserMatrix(m_3DViewer->getVtkVolume()->GetUserMatrix());

    // Add the new volume (initially hidden)
    m_3DMask->SetVisibility(false);
    m_3DViewer->getRenderer()->AddViewProp(m_3DMask);
}

void QDLSegmentationExtension::fillMaskExtentWithValue(int* extent, short value)
{
    // Get full mask dimensions and filling extent dimensions
    int* maskDims = m_maskData->GetDimensions();
    int fillDims[3] = {extent[1] - extent[0] + 1,
                       extent[3] - extent[2] + 1,
                       extent[5] - extent[4] + 1};

    // Get increments for jumping to next value when last value of extent along
    // X axis is reached (incY), and when last value of extent along Y axis is
    // reached (incZ)
    //  slice 1     slice 2
    // ...xxxY...  ...Zxxx...  incY to jump from Y to Y
    // ...Yxxx...  ...xxxx...  incZ to jump from Z to Z
    // ...xxxZ...  ...xxxx...
    int incY = maskDims[0] - fillDims[0];
    int incZ = (maskDims[1] - fillDims[1]) * maskDims[0];

    // Get pointer to mask scalars at the beginning of the filling extent
    short* maskData = static_cast<short*>(m_maskData->GetScalarPointer(extent[0], extent[2], extent[4]));

    // Iterate extent and set scalars to the value
    for (int z = extent[4]; z <= extent[5]; z++) {
        for (int y = extent[2]; y <= extent[3]; y++) {
            for (int x = extent[0]; x <= extent[1]; x++) {
                *maskData++ = value;
            }
            maskData += incY;
        }
        maskData += incZ;
    }
}

void QDLSegmentationExtension::fillMaskExtentWithData(int* extent, vtkImageData* imageData)
{
    // Get full mask dimensions and filling extent dimensions
    int* maskDims = m_maskData->GetDimensions();
    int fillDims[3] = {extent[1] - extent[0] + 1,
                       extent[3] - extent[2] + 1,
                       extent[5] - extent[4] + 1};

    // Get increments for jumping to next value when last value of extent along
    // X axis is reached (incY), and when last value of extent along Y axis is
    // reached (incZ)
    //  slice 1     slice 2
    // ...xxxY...  ...Zxxx...  incY to jump from Y to Y
    // ...Yxxx...  ...xxxx...  incZ to jump from Z to Z
    // ...xxxZ...  ...xxxx...
    int incY = maskDims[0] - fillDims[0];
    int incZ = (maskDims[1] - fillDims[1]) * maskDims[0];

    // Get pointers to mask scalars at the beginning of the filling extent and
    // to filling data scalars
    short* maskData = static_cast<short*>(m_maskData->GetScalarPointer(extent[0], extent[2], extent[4]));
    short* data = static_cast<short*>(imageData->GetScalarPointer());

    // Iterate extent and set scalars to the corresponding value in the filling
    // data
    for (int z = extent[4]; z <= extent[5]; z++) {
        for (int y = extent[2]; y <= extent[3]; y++) {
            for (int x = extent[0]; x <= extent[1]; x++) {
                *maskData++ = *data++;
            }
            maskData += incY;
        }
        maskData += incZ;
    }
}

void QDLSegmentationExtension::apply()
{
    // Get path of the trained model
    QString path;
    if (m_trainedModelCustomWidget->isVisible())
    {
        path = m_trainedModelCustomLineEdit->text();
    }
    else
    {
        int index = m_trainedModelPredefinedCombo->currentIndex();
        path = m_predefinedTrainedModels.at(index).second.path;
    }

    // Check the library used to train the deep-learning model and create the
    // segmentation object from the corresponding subclass
    if (path.endsWith(".pt")) {
        m_DLSegmentation = std::make_unique<DeepLearningTorchSegmentation>();
    }
    else {
        m_DLSegmentation = std::make_unique<DeepLearningTensorFlowSegmentation>();
    }


    // ********************************
    // * Prepare image for prediction *
    // ********************************

    // Get input image data
    vtkImageData* imageData = m_3DViewer->getMainInput()->getVtkData();

    // Get slice range to be segmented (currently restricted to current slice)
    int currentSlice = m_2DViewer->getViewer()->getCurrentSlice();
    m_sliceRange = {currentSlice, currentSlice};

    // Remove cropping area (if any) if it is placed on a different slice
    if (m_croppingAreaSlice != currentSlice) {
        removeCroppingArea();
    }

    // Get bounds according to cropping area and slice range
    QVector<int> bounds(6);
    getCroppingBounds(bounds);

    // Crop volume according to bounds
    vtkNew<vtkImageReslice> cropper;
    cropper->SetInputData(imageData);
    cropper->SetOutputOrigin(imageData->GetOrigin());
    cropper->SetOutputExtent(bounds.data());

    // Get model-view transform matrix from camera
    // (model-view transform matrix = concatenated transform matrix)
    vtkNew<vtkMatrix4x4> transform;
    transform->DeepCopy(m_2DViewer->getViewer()->getRenderer()->GetActiveCamera()->GetModelViewTransformMatrix());

    // Transform input data according to flips and rotations
    // (transform matrix concatenates flips and rotations)
    vtkNew<vtkImageReslice> reslicer;
    reslicer->SetInputConnection(cropper->GetOutputPort());
    reslicer->SetResliceAxes(transform);

    // Matrix is inverted because reslicer's formula is:
    // c_in = A c_out
    // where A is the reslice axes matrix, c_out is the output data coords and
    // c_in is the input data coords. Then:
    // A^-1 c_in = c_out
    reslicer->GetResliceAxes()->Invert();
    reslicer->Update();


    // Set input cropped image and other interface parameters
    m_DLSegmentation->setInput(reslicer->GetOutput());
    m_DLSegmentation->setModelInputDimensions(m_inputParamDimXSpinBox->value(),
                                             m_inputParamDimYSpinBox->value(),
                                             m_inputParamDimZSpinBox->value());
    m_DLSegmentation->setNormalisation(m_inputParamNormalisationCheckBox->isChecked());
    m_DLSegmentation->setNumberOfChannels(m_inputParamChannelsSpinBox->value());
    m_DLSegmentation->setNumberOfLabels(m_outputParamLabelsSpinBox->value());
    m_DLSegmentation->setActivationFunction(m_outputParamFunctionCombo->currentData()
                .value<DeepLearningSegmentation::ActivationFunction>());
    m_DLSegmentation->setSliceRange(m_sliceRange.data());
    m_DLSegmentation->setModelPath(path);


    // ******************************
    // * Predict and visualise mask *
    // ******************************

    try {
        // Perform segmentation
        m_DLSegmentation->predict();
    }
    catch (std::exception &e) {
        qDebug() << "ERROR: Prediction using the specified deep-learning model"
                 << "could not be performed (error below).";
        qDebug() << e.what();
        return;
    }

    // Get segmentation mask
    vtkImageData* mask = m_DLSegmentation->getOutputMask();

    // Restore orientation by inverting current transform matrix
    reslicer->SetInputData(mask);
    reslicer->GetResliceAxes()->Invert();
    reslicer->Update();
    mask = reslicer->GetOutput();


    // Replace the previous mask (if any) with the current one

    // Only scalars in previous mask extent are set to 0
    if (!m_maskExtent.empty()) {
        fillMaskExtentWithValue(m_maskExtent.data(), 0);
    }

//    // Alternative: all scalars to 0 (maybe less efficient?)
//    vtkDataArray::SafeDownCast(m_maskData->GetPointData()->GetAbstractArray(0))->Fill(0);

    // Set new scalars in current mask extent
    fillMaskExtentWithData(bounds.data(), mask);

    // Update current mask data and extent
    m_maskData->Modified();
    m_maskExtent = bounds;

    // Create a transfer function according to the number of output labels and
    // assign it to 2D and 3D mask volumes
    createTransferFunction();

    // Render the modified volume to the 2D and 3D viewers
    renderMask2D();
    renderMask3D();


    qDebug() << "Segmentation done!";
}

} // namespace udg
