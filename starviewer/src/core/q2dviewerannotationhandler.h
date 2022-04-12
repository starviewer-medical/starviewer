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

#include <QCoreApplication> // for Q_DECLARE_TR_FUNCTIONS

class vtkCornerAnnotation;
class vtkTextActor;

namespace udg {

class Q2DViewer;

/**
    Class to handle the annotations on a Q2DViewer.
    It setups and adds all the needed actors to the viewer since it's been created.
 */
class Q2DViewerAnnotationHandler {
    Q_DECLARE_TR_FUNCTIONS(Q2DViewerAnnotationHandler)

public:
    /// Returns a list of supported annotation variables with their corresponding descriptions.
    static const QVector<QPair<QString, QString>>& getSupportedAnnotations();

    explicit Q2DViewerAnnotationHandler(Q2DViewer *viewer);
    ~Q2DViewerAnnotationHandler();

    /// Enables or disables annotations according to the given boolean.
    void enableAnnotations(bool enable);

    /// Updates all annotations.
    void updateAnnotations();

private:
    /// Enumerated values to distinguish the corresponding index of each annotation
    enum CornerAnnotationIndexType { LowerLeftCornerIndex = 0, LowerRightCornerIndex = 1, UpperLeftCornerIndex = 2, UpperRightCornerIndex = 3 };
    enum OrientationLabelIndexType { LeftOrientationLabelIndex = 0, BottomOrientationLabelIndex = 1, RightOrientationLabelIndex = 2, TopOrientationLabelIndex = 3 };

private:
    /// Creates all the annotation actors
    void createAnnotations();

    /// Creates image orientation annotation actors
    void createOrientationAnnotations();

    /// Adds the text actors to the viewer
    void addActors();

    /// Updates the annotation in the specified corner.
    void updateCornerAnnotation(CornerAnnotationIndexType corner);
    /// Updates the image orientation labels.
    void updatePatientOrientationAnnotation();

    /// Returns the annotation template for the specified corner, retrieved from settings.
    QString getAnnotationTemplateForCorner(CornerAnnotationIndexType corner) const;

    /// Sets the specified text to the specified corner.
    void setCornerAnnotation(CornerAnnotationIndexType corner, QString text);

private:
    /// Viewer we are handling
    Q2DViewer *m_2DViewer;
    
    /// Actor to handle the corner annotations
    vtkCornerAnnotation *m_cornerAnnotations;

    /// Image orientation text actors
    vtkTextActor *m_patientOrientationTextActor[4];

    /// Indicates whether annotations are enabled or not.
    bool m_annotationsEnabled;
};

} // End namespace udg

#endif
