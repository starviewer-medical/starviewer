#ifndef UDGANNOTATIONFLAGS_H
#define UDGANNOTATIONFLAGS_H

namespace udg {

enum AnnotationFlag { NoAnnotation = 0x0, WindowInformationAnnotation = 0x1, PatientOrientationAnnotation = 0x2, SliceAnnotation = 0x8,
                        PatientInformationAnnotation = 0x10, AcquisitionInformationAnnotation = 0x20, AllAnnotation = 0x7F };
Q_DECLARE_FLAGS(AnnotationFlags, AnnotationFlag)

Q_DECLARE_OPERATORS_FOR_FLAGS(AnnotationFlags)

} // End namespace udg

#endif
