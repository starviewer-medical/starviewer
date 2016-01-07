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

#ifndef UDGANNOTATIONFLAGS_H
#define UDGANNOTATIONFLAGS_H

#include <QFlags>

namespace udg {

enum AnnotationFlag {
    NoAnnotation = 0x0,
    MainInformationAnnotation = 0x1,
    AdditionalInformationAnnotation = 0x2,
    VoiLutAnnotation = 0x4,
    SliceAnnotation = 0x8,
    PatientOrientationAnnotation = 0x10,
    AllAnnotations = MainInformationAnnotation | AdditionalInformationAnnotation | VoiLutAnnotation | SliceAnnotation | PatientOrientationAnnotation
};

Q_DECLARE_FLAGS(AnnotationFlags, AnnotationFlag)

Q_DECLARE_OPERATORS_FOR_FLAGS(AnnotationFlags)

} // End namespace udg

#endif
