#ifndef UDGPATIENTORIENTATION_H
#define UDGPATIENTORIENTATION_H

#include <QString>

namespace udg {

/**
    Classe que encapsularà l'atribut DICOM Patient Orientation (0020,0020)
  */
class PatientOrientation {
public:
    /// Etiquetes d'orientació
    static const QString LeftLabel;
    static const QString RightLabel;
    static const QString PosteriorLabel;
    static const QString AnteriorLabel;
    static const QString HeadLabel;
    static const QString FeetLabel;
};

} // End namespace udg

#endif
