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

#ifndef UDG_QDLSEGMENTATIONEXTENSION_H
#define UDG_QDLSEGMENTATIONEXTENSION_H

#include <QWidget>
#include <QPointer>

#include <vtkSmartPointer.h>
#include <vtkVolume.h>
#include <vtkMatrix4x4.h>

#include "ui_qdlsegmentationextensionbase.h"
#include "volume.h"
#include "drawerprimitive.h"
#include "drawerpolygon.h"

namespace udg {

// Rotation matrix 90 deg. clockwise
static constexpr double MATRIX_ROTATION_CW[16] = {
     0.0,  1.0,  0.0,  0.0,
    -1.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  1.0,  0.0,
     0.0,  0.0,  0.0,  1.0
};

// Rotation matrix 90 deg. counterclockwise
static constexpr double MATRIX_ROTATION_CCW[16] = {
     0.0, -1.0,  0.0,  0.0,
     1.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  1.0,  0.0,
     0.0,  0.0,  0.0,  1.0
};

// Horizontal flip matrix
static constexpr double MATRIX_FLIP_HORIZONTAL[16] = {
    -1.0,  0.0,  0.0,  0.0,
     0.0,  1.0,  0.0,  0.0,
     0.0,  0.0,  1.0,  0.0,
     0.0,  0.0,  0.0,  1.0
};

// Vertical flip matrix
static constexpr double MATRIX_FLIP_VERTICAL[16] = {
     1.0,  0.0,  0.0,  0.0,
     0.0, -1.0,  0.0,  0.0,
     0.0,  0.0,  1.0,  0.0,
     0.0,  0.0,  0.0,  1.0
};

class Patient;
class DeepLearningSegmentation; // needed to avoid include in header file
class DeepLearningTensorFlowSegmentation; // needed to avoid include
class DeepLearningTorchSegmentation; // needed to avoid include

class QDLSegmentationExtension : public QWidget, private ::Ui::QDLSegmentationExtensionBase
{

    Q_OBJECT

public:

    explicit QDLSegmentationExtension(QWidget *parent = nullptr);

    /// Set patient volume to viewers and initialise masks.
    void setPatient(Patient *patient);

private:
    /// Create connections between signals and slots.
    void createConnections();

    /// Initialise the tools available in the extension.
    void initializeTools();

    /// Perform action when a new predefined trained model is selected.
    void predefinedTrainedModelChanged(int index);

    /// Open dialog to select a custom trained model.
    void browseCustomTrainedModel();

    /// Restore 3D viewer to initial visualisation (without 3D mask).
    void restore3DViewer();

    /// Apply the trained model based on interface parameters.
    void apply();

    /// Manage drawer primitives for cropping.
    void primitiveUpdated(DrawerPrimitive* primitive, bool add);

    /// Get bounds (in structured coordinates of input image) of the currently
    /// defined cropping area.
    void getCroppingBounds(QVector<int>& bounds) const;

    /// Render the mask volume on the 2D viewer.
    void renderMask2D();

    /// Render the mask volume on the 3D viewer.
    void renderMask3D();

    /// Initialise the 2D viewer with the main volume and the 2D mask.
    void initialize2DViewer(Volume* mainVolume);

    /// Initialise the 3D viewer with the main volume and the 3D mask.
    void initialize3DViewer(Volume* mainVolume);


    /// Predefined trained models (name and path to model).
    QVector<QPair<QString, QString>> m_predefinedTrainedModels;
    /// Cropping area defined as a 2D polygon.
    QPointer<DrawerPolygon> m_croppingArea;
    /// Slice range to be segmented (both included).
    QVector<int> m_sliceRange;
    /// Mask data (stores voxel data).
    vtkSmartPointer<vtkImageData> m_maskData;
    /// Mask volume for 2D viewer (stores image properties and voxel data).
    QPointer<Volume> m_2DMask;
    /// Mask volume for 3D viewer (stores voxel data).
    vtkSmartPointer<vtkVolume> m_3DMask;
    /// Transform matrix to orient image as displayed on the viewer.
    vtkSmartPointer<vtkMatrix4x4> m_resliceAxesCosines;
    /// Deep-learning segmentation module.
    std::unique_ptr<DeepLearningSegmentation> m_DLSegmentation;

};

} // namespace udg

#endif // UDG_QDLSEGMENTATIONEXTENSION_H
