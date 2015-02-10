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

#ifndef UDGQ2DVIEWERANNOTATIONHANDLER_H
#define UDGQ2DVIEWERANNOTATIONHANDLER_H

#include <QString>
#include "annotationflags.h"

class vtkCornerAnnotation;
class vtkTextActor;

namespace udg {

class Q2DViewer;
class Series;

/**
    Class to handle the annotations on a Q2DViewer.
    The AnnotationFlags lets select which information will be displayed on each cornner of the viewer and the image orientation labels.
    It setups and adds all the needed actors to the viewer since it's been created.
 */
class Q2DViewerAnnotationHandler {
public:
    Q2DViewerAnnotationHandler(Q2DViewer *viewer);
    ~Q2DViewerAnnotationHandler();

    /// Enables the specified annotations. Unspecified annotations keep their current state.
    void enableAnnotations(AnnotationFlags annotations);
    /// Disables the specified annotations. Unspecified annotations keep their current state.
    void disableAnnotations(AnnotationFlags annotations);

    /// Updates the specified annotations.
    void updateAnnotations(AnnotationFlags annotations = AllAnnotations);

    /// Methods to update different kind of annotations
    void updateSliceAnnotationInformation();

    /// Updates image orientation labels
    void updatePatientOrientationAnnotation();

private:
    /// Refreshes the visibility of the annotations regarding the enabled flags
    void refreshAnnotations();

    /// Updates the main information annotation.
    void updateMainInformationAnnotation();
    /// Updates the additional information annotation.
    void updateAdditionalInformationAnnotation();
    
    /// Updates the slice annotations. It takes into account phases and slab thickness too
    void updateSliceAnnotation();

    void updateVoiLutInformationAnnotation();

    /// Returns the additional information in the general case.
    QString getStandardAdditionalInformation() const;
    /// Returns the additional information in the specific case of a MG image.
    QString getMammographyAdditionalInformation() const;
    /// Returns a label that describes the given series. Protocol and description information is used.
    QString getSeriesDescriptiveLabel(Series *series) const;
    
    /// Returns the current slice location information, if any
    QString getSliceLocationAnnotation();
    
    /// Creates all the annotation actors
    void createAnnotations();
    
    /// Creates image orientation annotation actors
    void createOrientationAnnotations();
    
    /// Adds the text actors to the viewer
    void addActors();

    /// Returns the current VOI LUT string.
    QString getVoiLutString() const;

private:
    /// Viewer we are handling
    Q2DViewer *m_2DViewer;
    
    /// Actor to handle the corner annotations
    vtkCornerAnnotation *m_cornerAnnotations;

    /// The strings for each corner annotation
    QString m_lowerLeftText;
    QString m_upperLeftText;

    /// Image orientation labels (Right,Left,Posterior,Anterior,Inferior,Superior)
    QString m_patientOrientationText[4];

    /// Image orientation text actors
    vtkTextActor *m_patientOrientationTextActor[4];

    /// Enumerated values to distinguish the corresponding index of each annotation
    enum CornerAnnotationIndexType { LowerLeftCornerIndex = 0, LowerRightCornerIndex = 1, UpperLeftCornerIndex = 2, UpperRightCornerIndex = 3 };
    enum OrienationLabelIndexType { LeftOrientationLabelIndex = 0, BottomOrientationLabelIndex = 1, RightOrientationLabelIndex = 2, TopOrientationLabelIndex = 3 }; 
    
    /// Flags to keep which annotations are enabled
    AnnotationFlags m_enabledAnnotations;
};

} // End namespace udg

#endif
