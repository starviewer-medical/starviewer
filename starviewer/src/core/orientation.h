#include <QString>

namespace udg {

/**
    Classe amb la que encapsularem els diferents tipus d'orientacions que podem
    manegar com poden ser els plans d'adquisició o la orienatació del pacient
  */
class Orientation {
public:
    /// Tipus d'orientacions del pla
    enum PlaneOrientationType { Axial, Sagittal, Coronal, Oblique, NotAvailable };

    /// Orientació del pacient
    static const QString LeftLabel;
    static const QString RightLabel;
    static const QString PosteriorLabel;
    static const QString AnteriorLabel;
    static const QString HeadLabel;
    static const QString FeetLabel;

    /// Donada una orientació de pla, ens retorna la corresponent etiqueta per mostrar a l'interfície
    static const QString getPlaneOrientationLabel(PlaneOrientationType orientation);
};

} // End namespace udg
