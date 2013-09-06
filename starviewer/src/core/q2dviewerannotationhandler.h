#ifndef UDGQ2DVIEWERANNOTATIONHANDLER_H
#define UDGQ2DVIEWERANNOTATIONHANDLER_H

#include <QString>
#include "annotationflags.h"

class vtkCornerAnnotation;
class vtkTextActor;

namespace udg {

class Q2DViewer;

/**
    Class to handle the annotations on a Q2DViewer.
    The AnnotationFlags lets select which information will be displayed on each cornner of the viewer and the image orientation labels.
    It setups and adds all the needed actors to the viewer since it's been created.
 */
class Q2DViewerAnnotationHandler {
public:
    Q2DViewerAnnotationHandler(Q2DViewer *viewer);
    ~Q2DViewerAnnotationHandler();

    /// Enables/disables visibility of the indicated annotations
    void enableAnnotation(AnnotationFlags annotation, bool enable = true);
    void removeAnnotation(AnnotationFlags annotation);

    /// Updates annotations data, all by default, otherwise only the specified ones
    void updateAnnotationsInformation(AnnotationFlags annotation = AllAnnotation);

    /// Methods to update different kind of annotations
    void updatePatientAnnotationInformation();
    void updateSliceAnnotationInformation();

    /// Updates image orientation labels
    void updatePatientOrientationAnnotation();

private:
    /// Refreshes the visibility of the annotations regarding the enabled flags
    void refreshAnnotations();
    
    /// Updates the slice annotations. It takes into account phases and slab thickness too
    void updateSliceAnnotation();

    void updateLateralityAnnotationInformation();
    void updatePatientInformationAnnotation();
    
    /// Creates all the annotation actors
    void createAnnotations();
    
    /// Creates image orientation annotation actors
    void createOrientationAnnotations();
    
    /// Adds the text actors to the viewer
    void addActors();

private:
    /// Viewer we are handling
    Q2DViewer *m_2DViewer;
    
    /// Actor to handle the corner annotations
    vtkCornerAnnotation *m_cornerAnnotations;

    /// The strings for each corner annotation
    QString m_lowerLeftText;
    QString m_lowerRightText;
    QString m_upperLeftText;
    QString m_upperRightText;

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
